#include "wifi_screen.h"
#include "internet_data.h"

static void UI_DrawSunnyIcon(void);
static void UI_DrawPartlyCloudyIcon(void);


void UI_WifiScreen_DrawBackground(void)
{
    // 1. ADIM: Tüm ekranı jilet gibi bir siyah yapalım, temizlik olsun
    ST7789_FillScreen(ST7789_COLOR_BLACK);

    // "İSTANBUL HAVA DURUMU" yazısı X=0, Y=5 koordinatından başlıyor
    ST7789_WriteString(0, 5, "ISTANBUL ", Font_11x18, 0xFDE0, ST7789_COLOR_BLACK); // Turuncu/Sarı heybetli başlık

    
    // ÇİZGİLER
    ST7789_DrawLine(0, 25, 240, 25, ST7789_COLOR_GRID);
    ST7789_DrawLine(0, 135, 240, 135, ST7789_COLOR_GRID);

    // 🌤️ REVIZE ADIM: HAVA DURUMU GÜNLERİ (25-135 ARASINA GÖRE ORTALI)
    ST7789_WriteString(33, 30, "BUGUN", Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    ST7789_WriteString(153, 30, "YARIN", Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);


    ST7789_DrawLine(120, 35, 120, 125, ST7789_COLOR_GRID);

    // 📊 5. ADIM: BORSA ALANI YATAY ÇİZGİLERİ (Altın, Gümüş, Dolar, Euro)
    ST7789_DrawLine(0, 161, 228, 161, ST7789_COLOR_GRID);
    ST7789_DrawLine(0, 187, 228, 187, ST7789_COLOR_GRID);
    ST7789_DrawLine(0, 213, 228, 213, ST7789_COLOR_GRID);    

    // 📈 6. ADIM: BORSA BAŞLIK YAZILARI
    ST7789_WriteString(0, 140, "ALTIN", Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    ST7789_WriteString(0, 166, "GUMUS", Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    ST7789_WriteString(0, 192, "DOLAR", Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    ST7789_WriteString(0, 220, "EURO",  Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
}


void UI_WifiScreen_UpdateClock(ds1307_dev_t *dev)
{
    if (ds1307_get_time(dev) == DS1307_OK)
    {
        ST7789_DrawRect(180, 5, 60, 18, ST7789_COLOR_BLACK); 

        // 2. ADIM: Saati String Formatına Getir (Sadece Saat ve Dakika)
        char clock_str[6]; // "HH:MM" + '\0' için 6 eleman yeterli
        sprintf(clock_str, "%02d:%02d", dev->time.hours, dev->time.minutes);

        // 3. ADIM: Saati sağ üst köşeye, ISTANBUL'un tam karşısına yazdır
        ST7789_WriteString(180, 5, clock_str, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    }
}




// GÜNEŞLİ İKONU
static void UI_DrawSunnyIcon(void)
{
    // Koordinatları direkt içeri çaktık hoca mk!
    uint16_t center_x = 60;
    uint16_t center_y = 68;
    uint16_t sun_color = 0xFFE0; // Güneş sarısı
    
    // Güneşin gövdesi (İçi dolu sarı daire)
    ST7789_DrawFilledCircle(center_x, center_y, 12, sun_color);
    
    // Güneşin Işınları
    ST7789_DrawLine(center_x, center_y - 20, center_x, center_y - 15, sun_color); // Üst
    ST7789_DrawLine(center_x, center_y + 15, center_x, center_y + 20, sun_color); // Alt
    ST7789_DrawLine(center_x - 20, center_y, center_x - 15, center_y, sun_color); // Sol
    ST7789_DrawLine(center_x + 15, center_y, center_x + 20, center_y, sun_color); // Sağ   
    
    // Çapraz Işınlar
    ST7789_DrawLine(center_x - 14, center_y - 14, center_x - 10, center_y - 10, sun_color);
    ST7789_DrawLine(center_x + 10, center_y + 10, center_x + 14, center_y + 14, sun_color);
    ST7789_DrawLine(center_x + 10, center_y - 10, center_x + 14, center_y - 14, sun_color);
    ST7789_DrawLine(center_x - 14, center_y + 14, center_x - 10, center_y + 10, sun_color);
}


// PARÇALI BULUTLU İKONU
static void UI_DrawPartlyCloudyIcon(void)
{
    uint16_t center_x = 180;
    uint16_t center_y = 70;

    uint16_t sun_color   = 0xFFE0;         // Parlak Sarı
    uint16_t cloud_color = 0xFFFF;         // Saf Beyaz Bulut
    uint16_t bg_color    = ST7789_COLOR_BLACK; // Arka plan (Siyah)

    // 1. ADIM: Arkadaki Güneşin Gövdesini Çiziyoruz (Sol yukarıda kalacak)
    int sun_x = center_x - 6;
    int sun_y = center_y - 6;
    ST7789_DrawFilledCircle(sun_x, sun_y, 10, sun_color); // Güneşin yuvarlağı
    
    // 2. ADIM: Sadece buluta girmeyecek, açıkta kalan güvenli ışınları çiziyoruz!
    ST7789_DrawLine(sun_x, sun_y - 17, sun_x, sun_y - 13, sun_color); // Üst Işın
    ST7789_DrawLine(sun_x - 17, sun_y, sun_x - 13, sun_y, sun_color); // Sol Işın
    ST7789_DrawLine(sun_x - 12, sun_y - 12, sun_x - 8, sun_y - 8, sun_color); // Sol-Üst Çapraz

    // 3. ADIM: BULUT ALANI MASKELEME (Kör Etme)
    int cloud_base_y = center_y + 8;
    ST7789_DrawFilledCircle(center_x - 10, cloud_base_y,     11, bg_color); // Sol yuva temizliği
    ST7789_DrawFilledCircle(center_x + 2,  cloud_base_y - 4, 14, bg_color); // Orta ana gövde temizliği
    ST7789_DrawFilledCircle(center_x + 14, cloud_base_y,     10, bg_color); // Sağ yuva temizliği
    ST7789_DrawLine(center_x - 22, cloud_base_y + 6, center_x + 24, cloud_base_y + 6, bg_color); // Taban çizgisi temizliği

    // 4. ADIM: BEYAZ POFUDUK BULUTU ÜSTÜNE ÇAKMA
    ST7789_DrawFilledCircle(center_x - 10, cloud_base_y,     8,  cloud_color); // Sol parça
    ST7789_DrawFilledCircle(center_x + 2,  cloud_base_y - 4, 11, cloud_color); // Orta büyük parça
    ST7789_DrawFilledCircle(center_x + 14, cloud_base_y,     7,  cloud_color); // Sağ küçük parça
    
    // Bulutun tabanını düzleştiren o jilet çizgiler
    ST7789_DrawLine(center_x - 18, cloud_base_y + 6, center_x + 20, cloud_base_y + 6, cloud_color);
    ST7789_DrawLine(center_x - 18, cloud_base_y + 7, center_x + 20, cloud_base_y + 7, cloud_color);
}



void UI_WifiScreen_Update(void)
{
    if (InternetData_Update()) 
    {

        char borsa_buf[32]; 


        char hava_buf[16];

        // Ana Derece (Örn: "24C") - X=43, Y=96 koordinatına basıyoruz hoca
        sprintf(hava_buf, "%.0fC", veriler.hava[0].deg); // .0f yaptık ki virgülden sonrası kafa açmasın mk
        ST7789_WriteString(43, 96, hava_buf, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK); 
        
        // Min / Max Dereceleri parçalı basıyoruz hoca (X=42, Y=118)
        sprintf(hava_buf, "%.0f", veriler.hava[0].min);
        ST7789_WriteString(42, 118, hava_buf, Font_7x10, ST7789_COLOR_BLUE, ST7789_COLOR_BLACK); // Min: Mavi
        
        ST7789_WriteString(56, 118, "/", Font_7x10, 0x7BEF, ST7789_COLOR_BLACK); // Ayraç: Gri
        
        sprintf(hava_buf, "%.0f", veriler.hava[0].max);
        ST7789_WriteString(63, 118, hava_buf, Font_7x10, ST7789_COLOR_RED, ST7789_COLOR_BLACK); // Max: Kırmızı


        //BÖLÜM: DINAMIK HAVA DURUMU (YARIN - i=1)
        sprintf(hava_buf, "%.0fC", veriler.hava[1].deg);
        ST7789_WriteString(163, 96, hava_buf, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK); 
        
        // Min / Max Dereceleri parçalı basıyoruz (X=162, Y=118)
        sprintf(hava_buf, "%.0f", veriler.hava[1].min);
        ST7789_WriteString(162, 118, hava_buf, Font_7x10, ST7789_COLOR_BLUE, ST7789_COLOR_BLACK); // Min: Mavi
        
        ST7789_WriteString(176, 118, "/", Font_7x10, ST7789_COLOR_GRID, ST7789_COLOR_BLACK); // Ayraç: Gri
        
        sprintf(hava_buf, "%.0f", veriler.hava[1].max);
        ST7789_WriteString(183, 118, hava_buf, Font_7x10, ST7789_COLOR_RED, ST7789_COLOR_BLACK); // Max: Kırmızı

        UI_DrawSunnyIcon();        
        UI_DrawPartlyCloudyIcon();  


        // 💰 1. ALTIN FİYATI (Y=140)
        // %.2f demek, virgülden sonra sadece 2 basamak al demektir mk (Örn: 2750.45)
        sprintf(borsa_buf, "%.2f", veriler.altin); 
        ST7789_WriteString(85, 140, borsa_buf, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK); 
        
        // 🥈 2. GÜMÜŞ FİYATI (Y=166)
        sprintf(borsa_buf, "%.2f", veriler.gumus);
        ST7789_WriteString(85, 166, borsa_buf, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK); 
        
        // 💵 3. DOLAR FİYATI (Y=192)
        sprintf(borsa_buf, "%.2f", veriler.dolar);
        ST7789_WriteString(85, 192, borsa_buf, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK); 
        
        // 💶 4. EURO FİYATI (Y=218)
        sprintf(borsa_buf, "%.2f", veriler.euro);
        ST7789_WriteString(85, 218, borsa_buf, Font_11x18, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
        // Not: Eğer borsa fiyatlarının yanına yukarı/aşağı ok işareti basacaksan, 
        // onları da X=200 civarına kondururuz, o iş kolay hoca.


      // ============================================================================
        // 📊 YENİ KISIM: İŞARETSİZ SADECE RENKLİ DEĞİŞİM MOTORU (BAĞIMSIZ DÖNGÜ)
        // ============================================================================
        char* degisimler[4] = {
            veriler.altin_degisim,  // Örn: "-0.78" (Kendi içinde dönerken bağımsız değerlendirilir)
            veriler.gumus_degisim,  // Örn: "-4.30"
            veriler.dolar_degisim,  // Örn: "+0.00"
            veriler.euro_degisim    // Örn: "+0.68"
        };
        
        uint16_t y_koordinatlari[4] = {140, 166, 192, 218}; // Senin milimetrik ekran koordinatların
        char yuzde_buf[16];

        for (int i = 0; i < 4; i++) 
        {
            uint16_t yuzde_renk = ST7789_COLOR_GREEN; // Her adımda renk yeşil olarak sıfırlanır hoca
            char* ham_yazi = degisimler[i];           // ham_yazi her dönüşte sadece o satırın verisini tutar

            // Eğer gelen veri boşsa veya geçersizse o satırı es geçiyoruz
            if (ham_yazi == NULL || ham_yazi[0] == '\0') {
                continue;
            }

            // 🧠 KARAKTER ANALİZİ VE RENKLENDİRME MOTORU
            if (ham_yazi[0] == '-') 
            {
                yuzde_renk = ST7789_COLOR_RED; // Sadece eksiyle başlayanlar KIRMIZI olur
                // (ham_yazi + 1) yazarak baştaki '-' karakterini pas geçip ekrandan siliyoruz hoca!
                sprintf(yuzde_buf, "%%%s", ham_yazi + 1); 
            } 
            else if (ham_yazi[0] == '+')
            {
                yuzde_renk = ST7789_COLOR_GREEN; // Artıyla başlayanlar veya sıfırlar YEŞİL olur
                // (ham_yazi + 1) yazarak baştaki '+' karakterini pas geçip ekrandan siliyoruz!
                sprintf(yuzde_buf, "%%%s", ham_yazi + 1);
            }
            else 
            {
                yuzde_renk = ST7789_COLOR_GREEN; // Başında hiçbir işaret olmayan düz yazılar YEŞİL olur
                sprintf(yuzde_buf, "%%%s", ham_yazi);
            }

            // Temizlenen işaretsiz yüzdeyi (Örn: %0.78 veya %4.30) kendi Y koordinatına şak diye basıyoruz!
            ST7789_WriteString(180, y_koordinatlari[i], yuzde_buf, Font_11x18, yuzde_renk, ST7789_COLOR_BLACK);
        }
    }
}

