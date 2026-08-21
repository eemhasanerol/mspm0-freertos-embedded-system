#include "app_init.h"


static ds1307_dev_t rtc;
bme280_dev_t bme = {0};
static qmc5883l_dev qmc_dev;

volatile ConnState_t g_connection_state = CONN_STATE_CHECKING;
volatile ScreenState_t g_current_screen = SCREEN_MAIN_OFFLINE;


TaskHandle_t xWifiInitTaskHandle = NULL;
TaskHandle_t xGuiTaskHandle = NULL;
TaskHandle_t xCompassTaskHandle = NULL;
TaskHandle_t xButtonTaskHandle = NULL;
TaskHandle_t xWatchdogTaskHandle = NULL;

EventGroupHandle_t xWatchdogEventGroup = NULL;
#define BIT_COMPASS_TASK    (1 << 0)  // 0x01
#define BIT_GUI_TASK        (1 << 1)  // 0x02
#define BIT_BUTTON_TASK     (1 << 2)  // 0x04

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
    
    SYSCFG_DL_WDT_init();

    NVIC_EnableIRQ(JOY_INT_IRQN);

    ST7789_Init();    // Ekran Başlatma
    clock_init();     // DS1307 Saat Başlatma
    bme_init();       // BME280 Sensör Başlatma
    qmc_init();  
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
    uint32_t expected_bits = 0;

    for (;;)
    {
        // Aktif ekrana göre hangi bitleri (kartları) bekleyeceğimizi dinamik seçiyoruz
        if (g_current_screen == SCREEN_COMPASS)
        {
            expected_bits = BIT_COMPASS_TASK | BIT_BUTTON_TASK;
        }
        else
        {
            expected_bits = BIT_GUI_TASK | BIT_BUTTON_TASK;
        }

        // Kartların tamamlanmasını maksimum 1.5 saniye bekliyoruz
        EventBits_t uxBits = xEventGroupWaitBits(
                                xWatchdogEventGroup,
                                expected_bits,
                                pdTRUE,        // Okununca bitleri otomatik temizle
                                pdTRUE,        // Beklenenlerin TÜMÜNÜN gelmesini bekle
                                pdMS_TO_TICKS(1500) // Zaman aşımı süresi
                             );

        // Eğer beklediğimiz tüm kartlar zamanında takıldıysa sistem sağlıklıdır
        if ((uxBits & expected_bits) == expected_bits)
        {
            DL_WWDT_restart(WDT_INST); // TI Donanımsal Watchdog'u besle!
        }
        else
        {
            // Eyvah! Görevlerden biri kilitlendi. Köpeği BESLEMİYORUZ.
            // Biz beslemeyi kesince 4 saniyelik donanım süresi dolacak ve işlemci reset yiyecek.
            while(1); 
        }
    }
}




static void vCompassTask(void *pvParameters)
{
    float heading = 0.0f;
    float old_heading = -999.0f; // İlk açılışta zorunlu çizim tetiklensin diye sahte bir başlangıç değeri veriyoruz
    for (;;)
    {
        if (qmc5883l_get_heading(&qmc_dev, &heading) == QMC5883L_OK)
        {
            if (fabsf(heading - old_heading) > 3.0f)
            {
                UI_CompassScreen_DrawNeedle(
                    old_heading,
                    ST7789_COLOR_BLACK
                );

                UI_CompassScreen_DrawNeedle(
                    heading,
                    ST7789_COLOR_RED
                );

                old_heading = heading;
            }
        }

        
        if (xWatchdogEventGroup != NULL) {
            xEventGroupSetBits(xWatchdogEventGroup, BIT_COMPASS_TASK);
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

// Kalıcı Sensör Güncelleme Thread'i
static void vGuiTask(void *pvParameters) {

    if (g_connection_state == CONN_STATE_OFFLINE) 
    {
        UI_OfflineScreen_DrawBackground();
    }else if(g_connection_state == CONN_STATE_WIFI_OK)
    {
        UI_WifiScreen_DrawBackground();
    }

    for (;;) 
    {
        // Sadece offline durum için doğrudan sensörleri güncelliyoruz
        if (g_connection_state == CONN_STATE_OFFLINE) 
        {
            UI_OfflineScreen_UpdateBME280(&bme);
            UI_OfflineScreen_UpdateClock(&rtc);
        }else if(g_connection_state == CONN_STATE_WIFI_OK){

            UI_WifiScreen_Update();
            UI_WifiScreen_UpdateClock(&rtc);
        }
        

        if (xWatchdogEventGroup != NULL) 
        {
            xEventGroupSetBits(xWatchdogEventGroup, BIT_COMPASS_TASK);
        }
        // 1 saniyede bir çalışması için gecikme
        vTaskDelay(pdMS_TO_TICKS(1000)); 
    }
}

void vButtonTask(void *pvParameters)
{
    for (;;)
    {
        // 1. Butona basılana kadar task burada uyur, işlemciyi asla yormaz
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        // --- DURUM A: ANA EKRANDAYIZ (Pusulaya geçeceğiz) ---
        if ((g_current_screen == SCREEN_MAIN_WIFI) || (g_current_screen == SCREEN_MAIN_OFFLINE))      
        {
            // Yeni ekran durumunu pusula yapıyoruz
            g_current_screen = SCREEN_COMPASS;
            
            // 1. Gui (Ekran) güncelleme task'ını durdur (Suspend)
            if (xGuiTaskHandle != NULL) {
                vTaskSuspend(xGuiTaskHandle);
            }

            // 2. Pusula statik arka planını çiz
            UI_CompassScreen_DrawBackground();

            // 3. Pusula güncelleme task'ını aktif et (Resume)
            if (xCompassTaskHandle != NULL) {
                vTaskResume(xCompassTaskHandle);
            }
        }
        // --- DURUM B: PUSULADAYIZ (Ana ekrana geri döneceğiz) ---
        else if (g_current_screen == SCREEN_COMPASS)
        {
            // 1. Pusula task'ını durdur (Suspend)
            if (xCompassTaskHandle != NULL) {
                vTaskSuspend(xCompassTaskHandle);
            }
      
            // 2. Wi-Fi bağlantı kontrolü yapıp uygun arka planı çiziyoruz hoca!
            if (g_connection_state == CONN_STATE_WIFI_OK)
            {
                g_current_screen = SCREEN_MAIN_WIFI;
                UI_WifiScreen_DrawBackground(); // Wifi'lı statik arka plan
            }
            else
            {
                g_current_screen = SCREEN_MAIN_OFFLINE;
                UI_OfflineScreen_DrawBackground(); // Çevrimdışı statik arka plan
            }  

            // 3. Gui (Ekran) güncelleme task'ını tekrar aktif et (Resume)
            if (xGuiTaskHandle != NULL) {
                vTaskResume(xGuiTaskHandle);
            }
        }

        if (xWatchdogEventGroup != NULL) 
        {
            xEventGroupSetBits(xWatchdogEventGroup, BIT_COMPASS_TASK);
        }
      
        // --- Debounce ve Kararlılık Bölümü (Donanım gürültüsünü önler) ---
        vTaskDelay(pdMS_TO_TICKS(100));
        while (DL_GPIO_readPins(JOY_PORT, JOY_BUTTON_PIN) == 0)
        {
            vTaskDelay(pdMS_TO_TICKS(20));
        }
        vTaskDelay(pdMS_TO_TICKS(100));

        // Kesme durumunu ve bildirimleri temizle
        DL_GPIO_clearInterruptStatus(JOY_PORT, JOY_BUTTON_PIN);
        xTaskNotifyStateClear(NULL);
    }
}





// --- Wi-Fi Kurulumunu Yapıp Kendi Kendini Silen Geçici Görev ---
static void vWifiInitTask(void *pvParameters) 
{
    
    g_connection_state = CONN_STATE_CHECKING;
    
    // Ekrana başlangıç bilgisini yazdır
    ST7789_FillScreen(ST7789_COLOR_BLACK);
    ST7789_WriteString(10, 100, "Sistem Baslatiliyor...", Font_7x10, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);

    // Wi-Fi bağlantısını dene (Bu sırada arkada RTOS kilitlenmez)
    bool wifi_baglandi = esp_wifi_kur_ve_baglan();
    delay_ms(10000);


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

    xTaskCreate(vButtonTask, "ButtonTask", 512, NULL, 5, &xButtonTaskHandle);
    
    xTaskCreate(vCompassTask, "CompassTask", 512, NULL, 4, &xCompassTaskHandle);
    if (xCompassTaskHandle != NULL) {
        vTaskSuspend(xCompassTaskHandle);
    }

    xTaskCreate(vGuiTask, "GuiTask", 512, NULL, 4, &xGuiTaskHandle);
    xTaskCreate(vWatchdogTask, "WatchdogTask", 512, NULL, 6, &xWatchdogTaskHandle);



    xWifiInitTaskHandle = NULL;
    vTaskDelete(NULL); 
}





// --- main.c Tarafından Çağrılan Tek Başlatıcı Nokta ---
void app_init(void) {
    // 1. Önce senin yazdığın donanım init fonksiyonunu çağırıyoruz
    periph_init();
    
    // 2. Ardından FreeRTOS zamanlayıcısı başlamadan önce Wi-Fi task'ını kuruyoruz
    xTaskCreate(vWifiInitTask, "WifiInitTask", 2048, NULL, 4, &xWifiInitTaskHandle);

    // 3. PUSULA TASK'I (Priority: 4)
}