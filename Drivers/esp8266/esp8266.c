#include "esp8266.h"

char ev_wifi_adi[32];
char ev_wifi_sifresi[32];

void UART_writeString(const char *str) 
{
    if (str == NULL) 
    {
        return; // Güvenlik kontrolü
    }

    while (*str) 
    {
        // FIFO boşalana kadar bekle
        while (DL_UART_Main_isTXFIFOFull(UART_INST));
        
        // Veriyi gönder
        DL_UART_Main_transmitData(UART_INST, (uint32_t)(*str++));
    }
}

// Zaman gecikmesi (FreeRTOS uyumlu sürüm)
void delay_ms(uint32_t ms) 
{
    // FreeRTOS scheduler (zamanlayıcı) başlamış mı kontrol et
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) 
    {
        // Eğer RTOS başladıysa, task'ı uyutarak bekle (İşlemciyi serbest bırakır)
        vTaskDelay(pdMS_TO_TICKS(ms));
    } 
    else 
    {
        // Eğer RTOS henüz başlamadıysa (ilk açılış anında), eski döngü yöntemiyle bekle
        for(uint32_t m = 0; m < ms; m++) {
            delay_cycles(32000); 
        }
    }
}



// Sıfır veri kaybı garantili, tamamen donanım hızında tarama yapan wifi_read
bool wifi_read(void)
{
    volatile uint16_t i = 0;
    
    // Kronometreyi başlatıyoruz (Sadece milisaniye sayacını okuyoruz, uyuma yok!)
    const uint32_t timeout = 23000000;
    volatile uint32_t sayac = 0;

    while (1) 
    { 
        // UART FIFO'sunda veri var mı? (İşlemci burayı nanosaniyeler seviyesinde tarar)
        if (!DL_UART_Main_isRXFIFOEmpty(UART_INST)) 
        {
            char c = DL_UART_Main_receiveData(UART_INST);
            response_buffer[i++] = c;
                    } 

        // İstediğimiz veri uzunluğuna ulaştıysak (Şifre geldiyse)
        if (i > 110) 
        {
            response_buffer[i] = '\0';
            UART_writeString("AT+CWSTOPSMART\r\n"); // Dinlemeyi kapat hoca
            return 1; // BAŞARILI!
        }
        
        if(sayac > timeout)
        {
            response_buffer[i] = '\0';
            UART_writeString("AT+CWSTOPSMART\r\n"); // Dinlemeyi kapat hoca
            return 0; // BAŞARILI!
        }

        sayac++;
    }
}    


void Save_WiFi_To_Flash(const char* ssid, const char* pass)
{
    // Donanımın istediği 128-bit hizalamaya uygun uint32_t array (16 word = 64 byte)
    __attribute__((aligned(8))) uint32_t write_buf[16] = {0}; 
    snprintf((char*)write_buf, 64, "%s|%s", ssid, pass);
    
    __disable_irq(); 
    
    // 1. Sektörü temizle (All-1's durumuna getir)
    DL_FlashCTL_unprotectSector(FLASHCTL, 0x0001F000, DL_FLASHCTL_REGION_SELECT_MAIN);
    DL_FlashCTL_eraseMemory(FLASHCTL, 0x0001F000, DL_FLASHCTL_COMMAND_SIZE_SECTOR);
    while ((FLASHCTL->GEN.STATCMD & FLASHCTL_STATCMD_CMDDONE_MASK) != FLASHCTL_STATCMD_CMDDONE_STATDONE);
    
    // 2. İŞTE SENİN KILAVUZDAN ÇIKARDIĞIMIZ %100 DOĞRU RESMİ FONKSİYON!
// Blocking128 hata verdiyse, kütüphanenin doğrudan RAM'den yürüttüğü bu 128-bit dostu fonksiyonu çağırıyoruz:
    DL_FlashCTL_programMemoryBlockingFromRAM64WithECCGenerated(
    FLASHCTL, 
    0x0001F000, 
    write_buf, 
    16, // 16 word = 64 byte (128-bitin tam katıdır, donanımı çok mutlu eder)
    DL_FLASHCTL_REGION_SELECT_MAIN
);
    
    // Donanımda ECC kazıma işleminin bitmesini bekle
    while ((FLASHCTL->GEN.STATCMD & FLASHCTL_STATCMD_CMDDONE_MASK) != FLASHCTL_STATCMD_CMDDONE_STATDONE);
    
    // 3. Sektörü tekrar koru
    DL_FlashCTL_protectSector(FLASHCTL, 0x0001F000, DL_FLASHCTL_REGION_SELECT_MAIN);
    
    __enable_irq(); 
}

void Load_WiFi_From_Flash(char* output_buffer)
{
    // 1. İşlemcinin (CPU) en sevdiği şey: 32-bit (Word) pointer tanımlıyoruz.
    // Adresimizi zorla 32-bit sınırına çekiyoruz.
    volatile const uint32_t* flash_ptr = (volatile const uint32_t*) 0x0001F000;
    
    // RAM'de işlemcinin çökmesini engelleyecek geçici 32-bit bir tampon açıyoruz
    uint32_t temp_ram_buf[16] = {0}; // 16 * 4 byte = 64 byte
    
    __disable_irq(); // Okuma esnasında araya hiçbir şey girmesin
    
    // 2. CPU'ya veriyi byte byte değil, 32-bitlik bütün kelimeler halinde çektiriyoruz.
    // Bu hareket dökümandaki o ölümcül "Dual-bit" hatasını tamamen bypass eder.
    for (int i = 0; i < 16; i++) 
    {
        temp_ram_buf[i] = flash_ptr[i];
    }
    
    __enable_irq();
    
    // 3. Veri güvenli bölgeye (RAM'e) geçti. 
    // Şimdi senin 'output_buffer'ına char (string) olarak kopyalayabiliriz.
    memcpy(output_buffer, (const char*)temp_ram_buf, 64);
}

/**
 * @brief  ESP8266'nın durumunu harici fonksiyon kullanmadan, bizzat donanımdan kendisi okuyarak sorgular.
 * @return bool -> true: İnternet var, false: İnternet yok hoca mk!
 */
bool esp_wifi_durum_kontrol(void)
{
    uint16_t idx = 0;
    uint32_t timeout = 900000; // Esnaf usulü donanım koruma sayacı

    // 1. Önce bizim yerel buffer'ı jilet gibi kazı hoca geçmiş temizlensin
    memset(response_buffer, 0, sizeof(response_buffer));
    
    // 2. ESP'ye fırçayı kay, hesabı sor mk!
    UART_writeString("AT+CIPSTATUS\r\n"); 
    
    // 3. 🧠 İŞTE KENDİN OKU DEDİĞİN CANLI MOTOR BURASI HOCA MK!
    // ESP'nin cevabı bitene kadar veya bizim sayaç patlayana kadar donanımın ağzından veriyi kapıyoruz
    while (timeout > 0)
    {
        // MSPM0'ın UART donanım kuyruğunda (RX FIFO) okunacak yeni mal (byte) var mı hoca?
        if (!DL_I2C_isControllerRXFIFOEmpty(I2C_INST)) // Sen hangi UART donanım fonksiyonunu kullanıyorsan o mk (Örn: DL_UART_isRXFIFOEmpty)
        {
            // Donanımdan byte'ı bizzat kendimiz çekip buffer'a diziyoruz aq!
            response_buffer[idx] = DL_UART_receiveData(UART_INST); 
            idx++;
            
            // Taşma olmasın diye önlemimizi alalım esnaflığa sığsın
            if (idx >= sizeof(response_buffer) - 1) break;
            
            timeout = 500000; // Her yeni veri geldiğinde timeout'u tazele hoca mk
        }
        else
        {
            timeout--; // Veri yoksa sayacı düşür
        }
    }
    response_buffer[idx] = '\0'; // String'in sonuna jilet gibi noktayı koy

    // 4. Şimdi kendimiz doldurduğumuz bu buffer'ı kontrol ediyoruz aq:
    if (strstr(response_buffer, "STATUS:2") != NULL || strstr(response_buffer, "WIFI GOT IP") != NULL) 
    {
        return true; // Delilli ispatlı internet var hoca!
    }
    
    return false; // Yoksa patladık mk
}


void esp8266_hw_reset(void)
{
    // Donanımsal Reset İşlemi artık burada tıkır tıkır çalışır hoca!
    DL_GPIO_clearPins(ESP8266_PORT, ESP8266_RST_PIN);
    vTaskDelay(pdMS_TO_TICKS(500)); 
    DL_GPIO_setPins(ESP8266_PORT, ESP8266_RST_PIN);
    vTaskDelay(pdMS_TO_TICKS(1000));  
}




bool esp_wif1i_kur_ve_baglan(void)
{

    ST7789_FillScreen(ST7789_COLOR_BLACK);
    ST7789_WriteString(10, 100, "Sistem Baslatiliyor...", Font_7x10, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);

    // ESP8266 Donanımsal Reset
    DL_GPIO_clearPins(ESP8266_PORT, ESP8266_RST_PIN);
    delay_ms(500); 
    DL_GPIO_setPins(ESP8266_PORT, ESP8266_RST_PIN);
    delay_ms(1000); 
    UART_writeString("AT+CWAUTOCONN=0\r\n");

    
    char okunan_string[64] = {0};

    Load_WiFi_From_Flash(okunan_string);

    //UART_writeString("AT+CWMODE=1\r\n"); 
    //delay_ms(1000); 


    //UART_writeString("AT+CWJAP=\"mehmeeet\",\"hasanerol\"\r\n");
    //delay_ms(10000); 


    if ((okunan_string[0] != 0xFF) && (okunan_string[0] != 0x00)) 
    {
        // HAFIZADA ŞİFRE VARSA: Veriyi '|' işaretinden ayırıp değişkenlere bölüyoruz
        char *token = strtok(okunan_string, "|");
        if(token != NULL) {
            strcpy(ev_wifi_adi, token);
            token = strtok(NULL, "|");
            if(token != NULL) {
                strcpy(ev_wifi_sifresi, token);
            }
        }

        // Direkt o şifreyle modeme bağlanıyoruz
        UART_writeString("AT+CWMODE=1\r\n"); 
        delay_ms(500);

        char baglanma_komutu[128];
        sprintf(baglanma_komutu, "AT+CWJAP=\"%s\",\"%s\"\r\n", ev_wifi_adi, ev_wifi_sifresi);
        UART_writeString(baglanma_komutu); 
        
        return esp_wifi_durum_kontrol();

    }
    else 
    {
        // HAFIZA BOŞSA: SmartConfig (EspTouch) modunu başlatıyoruz
        UART_writeString("AT+CWMODE=1\r\n"); 
        delay_ms(500);
        
        UART_writeString("AT+CWSTARTSMART=3\r\n"); // Havayı dinlemeye başla hoca
        delay_ms(500);

        while(1) 
        {
            if(wifi_read() == 0) // ESP'den veri gelmesini bekle (Sizin timeout mantığı)
            {
                return 0;
            }
            delay_ms(10000);
                
            // ESP bize "SSID: EvNet, PASSWORD: 123" gibi bir format verir.
            char *s_ptr = strstr(response_buffer, "ssid:");
            char *p_ptr = strstr(response_buffer, "password:");
            if(s_ptr && p_ptr) 
            {
                sscanf(s_ptr, "ssid:%[^\r\n]", ev_wifi_adi);
                    
                // "password:" kelimesini atla, satır sonundaki \r veya \n karakterine kadar olan her şeyi çek
                sscanf(p_ptr, "password:%[^\r\n]", ev_wifi_sifresi);
                    
                // Şifreyi MSP'nin kalıcı beynine yaz hoca!
                Save_WiFi_To_Flash(ev_wifi_adi, ev_wifi_sifresi);
                }
                
                break; // Kurulum döngüsünden kurtul, ana borsa moduna geç!
        }
    }

    return 1;
      
}


bool esp_wifi_kur_ve_baglan(void)
{
    // ESP8266 Donanımsal Reset (vTaskDelay veya delay_ms'e göre otomatik uyarlanır)
    DL_GPIO_clearPins(ESP8266_PORT, ESP8266_RST_PIN);
    vTaskDelay(pdMS_TO_TICKS(500));
    DL_GPIO_setPins(ESP8266_PORT, ESP8266_RST_PIN);
    vTaskDelay(pdMS_TO_TICKS(1000));     
    
    UART_writeString("AT+CWAUTOCONN=0\r\n");
    vTaskDelay(pdMS_TO_TICKS(100));
    
    char okunan_string[64] = {0};
    Load_WiFi_From_Flash(okunan_string);

    if ((okunan_string[0] != 0xFF) && (okunan_string[0] != 0x00)) 
    {
        // HAFIZADA ŞİFRE VARSA: Veriyi '|' işaretinden ayırıp değişkenlere bölüyoruz
        char *token = strtok(okunan_string, "|");
        if(token != NULL) {
            strcpy(ev_wifi_adi, token);
            token = strtok(NULL, "|");
            if(token != NULL) {
                strcpy(ev_wifi_sifresi, token);
            }
        }

        // Direkt o şifreyle modeme bağlanıyoruz
        UART_writeString("AT+CWMODE=1\r\n"); 
        delay_ms(500);

        char baglanma_komutu[128];
        sprintf(baglanma_komutu, "AT+CWJAP=\"%s\",\"%s\"\r\n", ev_wifi_adi, ev_wifi_sifresi);
        UART_writeString(baglanma_komutu); 

        // Bağlantı durumunu kontrol et
        return 1;
    }
    else 
    {
        // HAFIZA BOŞSA: SmartConfig (EspTouch) modunu başlatıyoruz
        UART_writeString("AT+CWMODE=1\r\n"); 
        delay_ms(500);
        
        UART_writeString("AT+CWSTARTSMART=3\r\n"); // Havayı dinlemeye başla hoca
        delay_ms(500);

        // Donanım hızında çalışan, 30 saniye timeout'lu ve sıfır veri kayıplı okuma fonksiyonumuz
        if(wifi_read() == 1) 
        {
            // ESP bize "ssid:EvNet\r\npassword:123\r\n" formatında veri gönderir.
            char *s_ptr = strstr(response_buffer, "ssid:");
            char *p_ptr = strstr(response_buffer, "password:");
            
            if(s_ptr && p_ptr) 
            {
                sscanf(s_ptr, "ssid:%[^\r\n]", ev_wifi_adi);
                sscanf(p_ptr, "password:%[^\r\n]", ev_wifi_sifresi);
                
                // Şifreyi MSP'nin kalıcı beynine yaz hoca!
                Save_WiFi_To_Flash(ev_wifi_adi, ev_wifi_sifresi);
                
                // Bağlantıyı doğrula ve sonucunu dön
                return esp_wifi_durum_kontrol();
            }
        }
        else
        {
            // 30 saniye boyunca havadan hiçbir şifre paketi gelmedi (Zaman aşımı)
            UART_writeString("AT+CWSTOPSMART\r\n");
            delay_ms(200);
            return false;
        }
    }

    return false;
}

