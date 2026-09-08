#include "app_init.h"


static ds1307_dev_t rtc;
bme280_dev_t bme = {0};
static qmc5883l_dev qmc_dev;

volatile ConnState_t g_connection_state = CONN_STATE_CHECKING;
volatile ScreenState_t g_current_screen = SCREEN_MAIN_OFFLINE;
static EventGroupHandle_t xWatchdogEventGroup = NULL;


#define BIT_GUI_TASK      (1U << 0)
#define BIT_COMPASS_TASK  (1U << 1)


TaskHandle_t xWifiInitTaskHandle = NULL;
TaskHandle_t xGuiTaskHandle = NULL;
TaskHandle_t xCompassTaskHandle = NULL;
TaskHandle_t xButtonTaskHandle = NULL;
TaskHandle_t xWatchdogTaskHandle = NULL;


static QueueHandle_t xCompassQueue = NULL;


void SYSCFG_DL_WWDT0_init(void)
{
    // SysConfig'in weak fonksiyonunu eziyoruz; içi boş olduğu için boot'ta başlamaz
}

void clock_init(void)
{
    rtc.dev_addr  = DS1307_I2C_ADDR;
    rtc.i2c_read  = platform_i2c_read;
    rtc.i2c_write = platform_i2c_write;

    rtc.time.seconds     = 40;
    rtc.time.minutes     = 3;
    rtc.time.hours       = 9;
    rtc.time.day_of_week = DS1307_MONDAY;
    rtc.time.date        = 21;
    rtc.time.month       = 5;
    rtc.time.year        = 26;
    rtc.time.time_format = DS1307_HOUR_24H;
    rtc.time.meridiem    = DS1307_AM;

    ds1307_init(&rtc);
    platform_delay_ms(500);
}

void bme_init(void)
{
    bme.dev_addr = BME280_I2C_ADDR_SDO_LOW;
    bme.osr_t    = BME280_OSR_T_2X;
    bme.osr_p    = BME280_OSR_P_4X;
    bme.osr_h    = BME280_OSR_H_1X;
    bme.filter   = BME280_FILTER_4;
    bme.standby  = BME280_STBY_1000_MS;
    bme.mode     = BME280_MODE_NORMAL;

    bme.i2c_read  = platform_i2c_read;
    bme.i2c_write = platform_i2c_write;
    bme.delay_ms  = platform_delay_ms;

    if (bme280_init(&bme) != BME280_OK) {
        while (1); 
    }

    platform_delay_ms(500);
}

void qmc_init(void)
{
    qmc_dev.dev_addr = QMC5883L_I2C_ADDR;
    qmc_dev.osr      = QMC5883L_OSR_512;
    qmc_dev.rng      = QMC5883L_RNG_2G;
    qmc_dev.odr      = QMC5883L_ODR_10HZ;
    qmc_dev.mode     = QMC5883L_MODE_CONTINUOUS;

    qmc_dev.i2c_read  = platform_i2c_read;
    qmc_dev.i2c_write = platform_i2c_write;
    qmc_dev.delay_ms  = platform_delay_ms;

    qmc_dev.calib.offset_x = 891.5f;
    qmc_dev.calib.offset_y = 208.5f;
    qmc_dev.calib.offset_z = 550.5f;
    
    /* Soft-iron scale factors */
    qmc_dev.calib.scale_x = 1.0f;
    qmc_dev.calib.scale_y = 1.0f;
    qmc_dev.calib.scale_z = 1.0f;

    // kontrol 
    if (qmc5883l_init(&qmc_dev) != QMC5883L_OK) 
    {
        while (1);
    }   

    platform_delay_ms(500);
}


void periph_init(void)
{
    SYSCFG_DL_init();
    NVIC_EnableIRQ(JOY_INT_IRQN);
    
    qmc_init();  
    bme_init();       // BME280 Sensör Başlatma
    clock_init();     // DS1307 Saat Başlatma

    ST7789_Init();    // Ekran Başlatma
}

void GROUP1_IRQHandler(void)
{
   if (DL_GPIO_getEnabledInterruptStatus(JOY_PORT, JOY_BUTTON_PIN))
   {
       DL_GPIO_clearInterruptStatus(JOY_PORT, JOY_BUTTON_PIN);

       BaseType_t xHigherPriorityTaskWoken = pdFALSE;

       vTaskNotifyGiveFromISR(xButtonTaskHandle, &xHigherPriorityTaskWoken);
       portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
   }
}

static void vWatchdogTask(void *pvParameters)
{
    EventBits_t required_bits;
    EventBits_t received_bits;

    DL_WWDT_initWatchdogMode(WWDT0_INST, DL_WWDT_CLOCK_DIVIDE_4,
        DL_WWDT_TIMER_PERIOD_18_BITS, DL_WWDT_STOP_IN_SLEEP,
        DL_WWDT_WINDOW_PERIOD_0, DL_WWDT_WINDOW_PERIOD_0);

    /* Set Window0 as active window */
    DL_WWDT_setActiveWindow(WWDT0_INST, DL_WWDT_WINDOW0);
    /* 3. Sayacı İLK KEZ burada tetikle (Sayaç şimdi geri saymaya başlar!) */
    DL_WWDT_restart(WWDT0_INST);
    
    for (;;)
    {

        if (g_current_screen == SCREEN_COMPASS)
                {
                    // Pusula ekranındayken hem GUI hem Pusula canlı olmalı
                    required_bits = BIT_GUI_TASK | BIT_COMPASS_TASK;
                }
                else
                {
                    // Ana ekranda sadece GUI canlı olsun yeter
                    required_bits = BIT_GUI_TASK;
                }

                // Bitleri bekle (pdFALSE: biz besleyene kadar silinmesin)
                received_bits = xEventGroupWaitBits(
                    xWatchdogEventGroup,
                    required_bits,
                    pdTRUE,
                    pdTRUE,              // required_bits içindeki TÜM bitler gelmeli (AND şartı)
                    pdMS_TO_TICKS(5000)
    );
    
        // Gerekli tüm bitler geldiyse donanımı besle ve temizle
        if ((received_bits & required_bits) == required_bits)
        {
            DL_WWDT_restart(WWDT0_INST);
            xEventGroupClearBits(xWatchdogEventGroup, required_bits);
        }

        /*
         * Eksik bit varsa hiçbir şey yapmıyoruz.
         * WWDT beslenmez.
         *
         * Problem devam ederse yaklaşık 4 saniye sonra
         * hardware reset oluşur.
         */
    }
}


static void vCompassTask(void *pvParameters)
{
    float heading = 0.0f;
    float old_heading = -999.0f;

    for (;;)
    {
        
        // 1. Eğer pusula ekranında DEĞİLSEK, bildirim gelene kadar UYU (0 CPU tüketimi)
        if (g_current_screen != SCREEN_COMPASS)
        {
            // Bildirim gelene kadar burada Blocked bekler, stack harcamaz
            ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
        }
        

        while (g_current_screen == SCREEN_COMPASS)
        {
            if (qmc5883l_get_heading(&qmc_dev, &heading) == QMC5883L_OK)
            {
                if (fabsf(heading - old_heading) > 3.0f)
                {
                    xQueueSend(xCompassQueue, &heading, 0);                    
                    old_heading = heading;
                }
            }
            xEventGroupSetBits(xWatchdogEventGroup, BIT_COMPASS_TASK);

            vTaskDelay(pdMS_TO_TICKS(50));  
        }

    }
}


// Kalıcı Sensör Güncelleme Thread'i
static void vGuiTask(void *pvParameters) 
{
    if (g_connection_state == CONN_STATE_OFFLINE) 
    {
        UI_OfflineScreen_DrawBackground();
    }else if(g_connection_state == CONN_STATE_WIFI_OK)
    {
        UI_WifiScreen_DrawBackground();
    }

    float compass_heading = 0.0f;
    float old_drawn_heading = 0.0f;

    ScreenState_t previous_screen = g_current_screen;

for (;;)
    {
        // EKRAN DEĞİŞMİŞ Mİ?
        if (g_current_screen != previous_screen)
        {
            switch (g_current_screen)
            {
                case SCREEN_COMPASS:
                    UI_CompassScreen_DrawBackground();
                    old_drawn_heading = 0.0f;
                    break;

                case SCREEN_MAIN_WIFI:
                    UI_WifiScreen_DrawBackground();
                    break;

                case SCREEN_MAIN_OFFLINE:
                    UI_OfflineScreen_DrawBackground();
                    break;

                default:
                    break;
            }

            previous_screen = g_current_screen;
        }

        switch (g_current_screen)
        {
            case SCREEN_COMPASS:

                if (xQueueReceive(xCompassQueue, &compass_heading, pdMS_TO_TICKS(20)) == pdPASS)
                {
                    UI_CompassScreen_DrawNeedle(
                        old_drawn_heading,
                        ST7789_COLOR_BLACK
                    );

                    UI_CompassScreen_DrawNeedle(
                        compass_heading,
                        ST7789_COLOR_RED
                    );

                    old_drawn_heading = compass_heading;
                }

                break;


            case SCREEN_MAIN_WIFI:

                UI_WifiScreen_Update();
                UI_WifiScreen_UpdateClock(&rtc);
                vTaskDelay(pdMS_TO_TICKS(2000));
                break;


            case SCREEN_MAIN_OFFLINE:

                UI_OfflineScreen_UpdateBME280(&bme);
                UI_OfflineScreen_UpdateClock(&rtc);

                vTaskDelay(pdMS_TO_TICKS(2000));
                break;

            default:

                vTaskDelay(pdMS_TO_TICKS(500));
                break;
        }

        xEventGroupSetBits(xWatchdogEventGroup, BIT_GUI_TASK);

    }
}

void vButtonTask(void *pvParameters)
{
    for (;;)
    {
        // Butona basılana kadar BLOCKED durumda bekle
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // ANA EKRAN -> PUSULA
        if ((g_current_screen == SCREEN_MAIN_WIFI) || (g_current_screen == SCREEN_MAIN_OFFLINE))
        {
            g_current_screen = SCREEN_COMPASS;

            // CompassTask'ı uyandır
            if (xCompassTaskHandle != NULL)
            {
                xTaskNotifyGive(xCompassTaskHandle);
            }
        }

        // PUSULA -> ANA EKRAN
        else if (g_current_screen == SCREEN_COMPASS)
        {
            if (g_connection_state == CONN_STATE_WIFI_OK)
            {
                g_current_screen = SCREEN_MAIN_WIFI;
            }
            else
            {
                g_current_screen = SCREEN_MAIN_OFFLINE;
            }
        }

        // Debounce
        vTaskDelay(pdMS_TO_TICKS(100));

        while (DL_GPIO_readPins(JOY_PORT, JOY_BUTTON_PIN) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20));
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}




// --- Wi-Fi Kurulumunu Yapıp Kendi Kendini Silen Geçici Görev ---
static void vWifiInitTask(void *pvParameters) 
{
    
    g_connection_state = CONN_STATE_CHECKING;
    
    // Ekrana başlangıç bilgisini yazdır
    ST7789_FillScreen(ST7789_COLOR_BLACK);
    ST7789_WriteString(10, 100, "Wi-Fi Araniyor", Font_7x10, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);

    // Wi-Fi bağlantısını dene (Bu sırada arkada RTOS kilitlenmez)
    bool wifi_baglandi = esp_wifi_kur_ve_baglan();
    delay_ms(10000);//500


    if (wifi_baglandi) {
        g_connection_state = CONN_STATE_WIFI_OK;
        g_current_screen = SCREEN_MAIN_WIFI;
        ST7789_WriteString(10, 120, "WiFi Baglantisi Basarili!", Font_7x10, ST7789_COLOR_GREEN, ST7789_COLOR_BLACK);
        vTaskDelay(pdMS_TO_TICKS(1000));
    } else {
        g_connection_state = CONN_STATE_OFFLINE;
        g_current_screen = SCREEN_MAIN_OFFLINE;

        ST7789_WriteString(10, 120, "Baglanti Yok! Offline Mod.", Font_7x10, ST7789_COLOR_RED, ST7789_COLOR_BLACK);
        vTaskDelay(pdMS_TO_TICKS(1500));
    }

    BaseType_t xReturned;
    // ButtonTask: 512 yerine 192 word (Yaklaşık 768 byte, buton için fazlasıyla yeterli)
    xReturned = xTaskCreate(vButtonTask, "ButtonTask", 192, NULL, 5, &xButtonTaskHandle);

        if (xReturned != pdPASS)
    {
        // EĞER KOD BURAYA GİRİYORSA: 
        // Kesinlikle Heap yetmemiştir (tek parça 3200 byte bulunamamıştır).
        // Buraya bir breakpoint koyup durdurabilirsiniz!
        while(1); 
    }

    // GuiTask: 512 yerine 384 word (1536 byte, ekrana çizim için güvenli pay bırakır)
    xReturned = xTaskCreate(vGuiTask, "GuiTask", 800, NULL, 4, &xGuiTaskHandle);
    if (xReturned != pdPASS)
{
    // EĞER KOD BURAYA GİRİYORSA: 
    // Kesinlikle Heap yetmemiştir (tek parça 3200 byte bulunamamıştır).
    // Buraya bir breakpoint koyup durdurabilirsiniz!
    while(1); 
}
    // CompassTask: 512 yerine 768 veya 1024 word (Taşmayı kesin çözer)
    xReturned = xTaskCreate(vCompassTask, "CompassTask", 512, NULL, 4, &xCompassTaskHandle);
if (xReturned != pdPASS)
{
    // EĞER KOD BURAYA GİRİYORSA: 
    // Kesinlikle Heap yetmemiştir (tek parça 3200 byte bulunamamıştır).
    // Buraya bir breakpoint koyup durdurabilirsiniz!
    while(1); 
}
    // WatchdogTask: 128 word (512 byte) gayet iyi, unused değeri 260 byte ile dengeli
    xReturned = xTaskCreate(vWatchdogTask, "WatchdogTask", 128, NULL, 6, &xWatchdogTaskHandle);
if (xReturned != pdPASS)
{
    // EĞER KOD BURAYA GİRİYORSA: 
    // Kesinlikle Heap yetmemiştir (tek parça 3200 byte bulunamamıştır).
    // Buraya bir breakpoint koyup durdurabilirsiniz!
    while(1); 
}


    xWifiInitTaskHandle = NULL;
    vTaskDelete(NULL); 
}



 

// --- main.c Tarafından Çağrılan Tek Başlatıcı Nokta ---
void app_init(void) {
    // 1. Önce senin yazdığın donanım init fonksiyonunu çağırıyoruz
    periph_init();

    xCompassQueue = xQueueCreate(1, sizeof(float));

    if (xCompassQueue == NULL)
    {
        while (1);
    }

    xWatchdogEventGroup = xEventGroupCreate();

    if (xWatchdogEventGroup == NULL)
    {
        while (1);
    }
    
    // 2. Ardından FreeRTOS zamanlayıcısı başlamadan önce Wi-Fi task'ını kuruyoruz
    xTaskCreate(vWifiInitTask, "WifiInitTask", 2048, NULL, 4, &xWifiInitTaskHandle);

    // 3. PUSULA TASK'I (Priority: 4)
}