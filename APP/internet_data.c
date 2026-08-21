#include "internet_data.h"

PiyasaVerileri veriler;


static void InternetData_Parse(const char *raw_data)
{
    char *ptr;
    char anahtar[32];

    // --- BORSA VERİLERİ (ÇALIŞAN OLAN TAM FORMATI) ---
    ptr = strstr(raw_data, "\"dolar\":");
    if (ptr) 
    {
        sscanf(ptr, "\"dolar\":%f", &veriler.dolar);
    }

    ptr = strstr(raw_data, "\"altin\":");
    if (ptr) 
    {
        sscanf(ptr, "\"altin\":%f", &veriler.altin);
    }


    ptr = strstr(raw_data, "\"euro\":");
    if (ptr) 
    {
        sscanf(ptr, "\"euro\":%f", &veriler.euro);
    }


    ptr = strstr(raw_data, "\"gumus\":");
    if (ptr) 
    {
        sscanf(ptr, "\"gumus\":%f", &veriler.gumus);
    }


    // --- 📊 YENİ KISIM: YÜZDELİK DEĞİŞİM STRINGLERİNİ PARSE ETME (TIRNAKLI FORMAT) ---
    ptr = strstr(raw_data, "\"dolar_degisim\":\"");
    if (ptr) 
    {
        // "dolar_degisim":"+0.00" tırnak içerisindeki kısmı okur ve veriler.dolar_degisim'e yazar hoca
        sscanf(ptr, "\"dolar_degisim\":\"%[^\"]\"", veriler.dolar_degisim);
    }

    ptr = strstr(raw_data, "\"euro_degisim\":\"");
    if (ptr) 
    {
        sscanf(ptr, "\"euro_degisim\":\"%[^\"]\"", veriler.euro_degisim);
    }

    ptr = strstr(raw_data, "\"altin_degisim\":\"");
    if (ptr) 
    {
        sscanf(ptr, "\"altin_degisim\":\"%[^\"]\"", veriler.altin_degisim);
    }

    ptr = strstr(raw_data, "\"gumus_degisim\":\"");
    if (ptr) 
    {
        sscanf(ptr, "\"gumus_degisim\":\"%[^\"]\"", veriler.gumus_degisim);
    }

    // --- 3 GÜNLÜK HAVA DURUMU DÖNGÜSÜ (TIRNAKLI VE İKİ NOKTALI FORMAT) ---
    for (int i = 0; i < 2; i++) 
    {
        int gun_no = i + 1; // JSON'da d1, d2, d3 diye gidiyor hoca

        // Ortalama Dereceyi Bul ve Oku (Örn: "d1_deg":"23")
        sprintf(anahtar, "\"d%d_deg\":\"", gun_no);
        ptr = strstr(raw_data, anahtar);
        if (ptr) 
        {
            // İki noktadan sonraki tırnak içindeki float sayıyı okur hoca
            sscanf(ptr, "\"d%*d_deg\":\"%f\"", &veriler.hava[i].deg);
        }

        // Max Dereceyi Bul ve Oku (Örn: "d1_max":"27")
        sprintf(anahtar, "\"d%d_max\":\"", gun_no);
        ptr = strstr(raw_data, anahtar);
        if (ptr) 
        {
            sscanf(ptr, "\"d%*d_max\":\"%f\"", &veriler.hava[i].max);
        }

        // Min Dereceyi Bul ve Oku (Örn: "d1_min":"14")
        sprintf(anahtar, "\"d%d_min\":\"", gun_no);
        ptr = strstr(raw_data, anahtar);
        if (ptr) 
        {
            sscanf(ptr, "\"d%*d_min\":\"%f\"", &veriler.hava[i].min);
        }

        // Durumu Bul ve Oku (Örn: "d1_dur":"acik")
        sprintf(anahtar, "\"d%d_dur\":\"", gun_no);
        ptr = strstr(raw_data, anahtar);
        if (ptr) 
        {
            // Tırnak kapanana kadar olan string metni çeker hoca
            sscanf(ptr, "\"d%*d_dur\":\"%[^\"]\"", veriler.hava[i].dur);
        }

    }
}


bool InternetData_Update(void)
{
     // İnternetteki 7/24 açık sunucumuza TCP port 80'den bağlanıyoruz
    UART_writeString("AT+CIPSTART=\"TCP\",\"erolsaat.pythonanywhere.com\",80\r\n");
    delay_ms(2000);

    // Sunucuya tam 76 karakterlik veri göndereceğimizi bildiriyoruz
    UART_writeString("AT+CIPSEND=78\r\n");
    delay_ms(2000);

    UART_writeString("GET /piyasa HTTP/1.1\r\n"
                    "Host: erolsaat.pythonanywhere.com\r\n"
                    "Connection: close\r\n\r\n");     
    delay_ms(500);


       
    // --- 1. DEĞİŞKEN TANIMLAMALARI (En başta olmak zorunda!) ---
    char response_buffer[560]; 
    volatile int i = 0;
    volatile int last_char_time = 0;
    const uint32_t timeout_limit = 2000000;



    while (1) 
    { 
        if (!DL_UART_Main_isRXFIFOEmpty(UART_INST)) {
            char c = DL_UART_Main_receiveData(UART_INST);
            
            response_buffer[i++] = c;
            
        } else {
            last_char_time++; // Veri gelmedi, sayacı artır
        }

        if ((last_char_time > timeout_limit) || ( i > 559)) {
            response_buffer[i] = '\0';
            break;
        }
    }


    if(i > 500)
    {
        InternetData_Parse(response_buffer);
        return true;
    }

    return false;
}
