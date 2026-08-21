#include "offline_screen.h"


static void UI_DrawThermometerIcon(uint16_t x, uint16_t y, uint16_t color);
static void UI_DrawDropletIcon(uint16_t x, uint16_t y, uint16_t color);

void UI_OfflineScreen_DrawBackground(void)
{
    // 1. ARKA PLAN TAMAMEN SAF SİYAH
    ST7789_FillScreen(ST7789_COLOR_BLACK);
    
    ST7789_WriteString(56, 2, "ISTANBUL", Font_16x26, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);


    
    uint16_t ST7789_COLOR_GRAY = 0x7BEF; 

    // ==========================================
    // YENİ EKLEME: SAATİN ÜSTÜNDEKİ YATAY GRİ ÇİZGİ (Daha Yukarıda ve 4 Piksel Kalın)
    // ==========================================
    // Çizgi Y=85 koordinatına taşındı ve 4 katman atılarak kalınlaştırıldı
    ST7789_DrawLine(2, 85, 238, 85, ST7789_COLOR_GRAY);
    ST7789_DrawLine(2, 86, 238, 86, ST7789_COLOR_GRAY);
    ST7789_DrawLine(2, 87, 238, 87, ST7789_COLOR_GRAY);
    ST7789_DrawLine(2, 88, 238, 88, ST7789_COLOR_GRAY);
    
    // ==========================================
    // SAATİN ALTINDAKİ YATAY ÇİZGİ (4 Piksel Kalın Gri)
    // ==========================================
    // Çizgi aşağıya doğru 4 piksel kalınlığa ulaştırıldı
    ST7789_DrawLine(2, 153, 238, 153, ST7789_COLOR_GRAY);
    ST7789_DrawLine(2, 154, 238, 154, ST7789_COLOR_GRAY);
    ST7789_DrawLine(2, 155, 238, 155, ST7789_COLOR_GRAY);
    ST7789_DrawLine(2, 156, 238, 156, ST7789_COLOR_GRAY); 
    
    // =========================================    =
    // EN ALT KATIN ORTASINDAKİ DİKEY BÖLÜCÜ ÇİZGİ (4 Piksel Kalın Gri)
    // ==========================================
    // X ekseninde sağa doğru 4 piksel genişletildi
    ST7789_DrawLine(119, 157, 119, 235, ST7789_COLOR_GRAY);
    ST7789_DrawLine(120, 157, 120, 235, ST7789_COLOR_GRAY);
    ST7789_DrawLine(121, 157, 121, 235, ST7789_COLOR_GRAY);
    ST7789_DrawLine(122, 157, 122, 235, ST7789_COLOR_GRAY);
    // ==========================================
    // 4. ALT KAT: DEVASA GEOMETRİK İKONLAR
    // ==========================================
    // Sol Alt: Kocaman Kırmızı Termometre (~28x48 px)
    UI_DrawThermometerIcon(2, 172, ST7789_COLOR_RED);
    
    // Sağ Alt: Kocaman Mavi Su Damlası (~22x32 px)
    // Hizayı eşitlemek için dikeyde Y=180 yapıldı
    //UI_DrawDropletIcon(126, 180, ST7789_COLOR_BLUE);

    UI_DrawDropletIcon(210, 182, ST7789_COLOR_BLUE);

    //2 NOKTA ::
    ST7789_DrawChar_Font16x26_Scaled2x(104, 100, ':', ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
}
    
// Termometre İkonu
void UI_DrawThermometerIcon(uint16_t x, uint16_t y, uint16_t color)
{
    // 1. DIŞ ÇERÇEVE (2 Kat Genişletilmiş)
    ST7789_DrawCircle(x + 12, y + 6, 6, color); // Üst yuvarlak kafa
    
    // Sol ve sağ dikey kalınlaştırılmış cam duvarlar
    ST7789_DrawLine(x + 6, y + 6,  x + 6,  y + 28, color);
    ST7789_DrawLine(x + 7, y + 6,  x + 7,  y + 28, color);
    ST7789_DrawLine(x + 17, y + 6, x + 17, y + 28, color);
    ST7789_DrawLine(x + 18, y + 6, x + 18, y + 28, color);
    
    // Alttaki büyük cıva haznesi (Yarıçap: 10)
    ST7789_DrawCircle(x + 12, y + 36, 10, color);
    ST7789_DrawCircle(x + 12, y + 36, 9, color);

    // 2. İÇ DOLGU (Kırmızı Cıva Sütunu)
    // Haznenin içini dolduran katmanlar
    ST7789_DrawRect(x + 6,  y + 32, 13, 2, color);
    ST7789_DrawRect(x + 4,  y + 34, 17, 2, color);
    ST7789_DrawRect(x + 3,  y + 36, 19, 4, color); // Hazne göbeği
    ST7789_DrawRect(x + 4,  y + 40, 17, 2, color);
    ST7789_DrawRect(x + 6,  y + 42, 13, 2, color);
    
    // Yukarı uzanan kalın cıva çizgisi
    ST7789_DrawRect(x + 10, y + 10, 5, 20, color);
}// Slayttaki Su Damlası İkonu (💧) - Boyut: ~11x18 piksel uyumlu


// Damla İkonu
void UI_DrawDropletIcon(uint16_t x, uint16_t y, uint16_t color)
{
    // Tepe noktası ve genişleyen üçgen çatı
    ST7789_DrawPixel(x + 10, y, color);
    ST7789_DrawRect(x + 9,  y + 1,  3, 2, color);
    ST7789_DrawRect(x + 8,  y + 3,  5, 3, color);
    ST7789_DrawRect(x + 6,  y + 6,  9, 4, color);
    ST7789_DrawRect(x + 4,  y + 10, 13, 4, color);
    
    // Geniş gövde ve yuvarlak alt taban dolgusu
    ST7789_DrawRect(x + 2,  y + 14, 17, 10, color);
    ST7789_DrawRect(x + 4,  y + 24, 13, 4, color);
    ST7789_DrawRect(x + 6,  y + 28, 9, 2, color);
    ST7789_DrawRect(x + 8,  y + 30, 5, 1, color);
}




void UI_OfflineScreen_UpdateBME280(bme280_dev_t *dev)
{
    bme280_data_t sensor_data;
    char tempStr[20];
    char humStr[20];
    
    // BME280 sensöründen tüm verileri oku
    if (bme280_read_all(dev, &sensor_data) == BME280_OK) {
        
        // ====================================================
        // 1. SICAKLIK HESAPLAMA VE AYARLAMA (Sola Yaslı & Aşağı Alındı)
        // ====================================================
        int16_t temp_whole = (int16_t)sensor_data.temperature_c;
        uint16_t temp_frac = (uint16_t)((sensor_data.temperature_c - temp_whole) * 10);
        if (temp_frac > 9) temp_frac = 9;
        
        // String formatlama (Örn: "24.5C")
        sprintf(tempStr, "%d.%dC", temp_whole, temp_frac);
        
        // Kırmızı termometrenin yanı (Sabit X=30)
        int startX_temp = 30; 
        
        // Temizleme alanını dikeyde yazıya göre hizaladık (Y=182)
        ST7789_DrawRect(28, 182, 90, 30, ST7789_COLOR_BLACK);
        
        // Dikey koordinat 179'dan 186'ya indirilerek biraz daha aşağı alındı
        ST7789_WriteString(startX_temp, 186, tempStr, Font_16x26, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);


        // 2. NEM HESAPLAMA VE AYARLAMA (Sağa Doğru İyice Yanaştırıldı)
        // ====================================================
        uint16_t hum_whole = (uint16_t)sensor_data.humidity_rh;
        if (hum_whole > 99) hum_whole = 99;
        
        // String formatlama (Örn: "%58")
        sprintf(humStr, "%%%d", hum_whole);
        
        int hum_len = strlen(humStr);
        // Yazıyı sağdaki mavi damlaya (X=210) iyice yanaştırmak için formülü X=208 yaptık.
        // Yazı artık sola doğru fazla taşmayacak, damlanın hemen solunda jilet gibi duracak.
        int startX_hum = 190 - (hum_len * 16); 
        
        // Temizleme alanını fazla soldan başlatmıyoruz! X=122 yerine X=145'e çektik.
        // Böylece orta çizginin sağındaki o boş alan gereksiz yere silinmez ve yazı tam yerinde temizlenir.
        // Bitişini de X=208 yaparak X=210'daki mavi damlayı korumaya devam ediyoruz.
        ST7789_DrawRect(145, 182, 63, 32, ST7789_COLOR_BLACK);
        
        // Nem yazısını sıcaklıkla tam paralel dikey hizada basıyoruz (Y=186)
        ST7789_WriteString(startX_hum, 186, humStr, Font_16x26, ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    }
}


static void UI_UpdateClockArea(ds1307_dev_t *dev)
{
    // 1. ADIM: Eski saatin alanını siyah kutuyla temizle
    // Ekran genişliğine göre ortalanmış temizleme alanı (X=40, Y=100, Genişlik=160, Yükseklik=53)
    //ST7789_DrawRect(40, 100, 160, 53, ST7789_COLOR_BLACK); 
    
    ST7789_DrawRect(40, 100, 64, 53, ST7789_COLOR_BLACK); 
    
    // Dakika rakamlarının alanı (X=136'dan başlar, 64 piksel genişlik -> X=200'e kadar temizler)
    // Böylece X=104 ile X=136 arasında duran iki nokta (:) bölgesine ASLA DOKUNMAZ!
    ST7789_DrawRect(136, 100, 64, 53, ST7789_COLOR_BLACK);


    uint16_t current_x = 40;  // Tam ortalanmış başlangıç X koordinatı
    uint16_t current_y = 100; // 155'teki çizginin hemen üstü (100 + 52 = 152 dikey sınır)
    
    // 2. ADIM: SAAT ONLAR VE BİRLER BASAMAĞI
    ST7789_DrawChar_Font16x26_Scaled2x(current_x, current_y, (dev->time.hours / 10) + '0', ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    current_x += 32;
    
    ST7789_DrawChar_Font16x26_Scaled2x(current_x, current_y, (dev->time.hours % 10) + '0', ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    current_x += 32;
    
    // 3. ADIM: İKİ NOKTA ÜST ÜSTE
    //ST7789_DrawChar_Font16x26_Scaled2x(current_x, current_y, ':', ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    current_x += 32;
    
    // 4. ADIM: DAKİKA ONLAR VE BİRLER BASAMAĞI
    ST7789_DrawChar_Font16x26_Scaled2x(current_x, current_y, (dev->time.minutes / 10) + '0', ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
    current_x += 32;
    
    ST7789_DrawChar_Font16x26_Scaled2x(current_x, current_y, (dev->time.minutes % 10) + '0', ST7789_COLOR_WHITE, ST7789_COLOR_BLACK);
}



static void UI_UpdateDateArea(ds1307_dev_t *dev)
{
    char line1[30];
    const char* monthName = GetMonthName(dev->time.month);
    const char* dayName = GetDayName(dev->time.day_of_week);
    
    // 1. Satır: Tarih (Örn: 22 MAYIS 2026)
    // Eğer dev->time.year zaten 2026 şeklinde 4 haneli geliyorsa başındaki "20"yi silebilirsin abi.
    sprintf(line1, "%02d %s 20%02d", dev->time.date, monthName, dev->time.year);
    
    // Satır uzunluklarını hesapla (11x18 fontunda 1 karakter 11 piksel genişliğinde)
    int len1 = strlen(line1);
    int len2 = strlen(dayName);
    
    // Ekranın tam ortası 120 piksel. Karakter genişliği 11 piksele göre X başlangıcını otomatik ortalar:
    int startX1 = 120 - ((len1 * 11) >> 1);
    int startX2 = 120 - ((len2 * 11) >> 1);
    
    // 2. ADIM: İstanbul'un altı ile gri çizginin (Y=85) üstündeki alanı temizle
    // İstanbul yazısı Y=28'de bitiyor, temizleme kutusunu Y=32 ile Y=82 arasına (50 piksel yükseklikte) açtık.
    ST7789_DrawRect(15, 32, 210, 50, ST7789_COLOR_BLACK);
    
    // 3. ADIM: Yazıları jilet gibi yerleştir
    // Tarih satırı Y=35, Gün ismi satırı Y=58 koordinatında tam ortalı basılır
    ST7789_WriteString(startX1, 35, line1, Font_11x18, 0xAEDC, ST7789_COLOR_BLACK);   // Turkuaz renk çok yakışır
    ST7789_WriteString(startX2, 58, dayName, Font_11x18, 0xAEDC, ST7789_COLOR_BLACK); // Turkuaz renk
}


// rtc cihazından okuma yapıp saat ve tarihi birlikte günceller
void UI_OfflineScreen_UpdateClock(ds1307_dev_t *dev)
{
    if (ds1307_get_time(dev) == DS1307_OK)
    {
        UI_UpdateClockArea(dev);
        UI_UpdateDateArea(dev);
    }
}



