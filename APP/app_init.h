#ifndef APP_INIT_H
#define APP_INIT_H


#include "ti_msp_dl_config.h"
#include "platform.h"
#include "event_groups.h" // Event group kütüphanesi


#include "compass_screen.h"
#include "offline_screen.h"
#include "wifi_screen.h"

// 5) Global Durum Yapıları (Enums)
typedef enum {
    CONN_STATE_CHECKING,   
    CONN_STATE_WIFI_OK,    
    CONN_STATE_OFFLINE     
} ConnState_t;

typedef enum {
    SCREEN_MAIN_WIFI,      // 1. Ekran: İnternetli (Saat + Borsa + Hava Durumu)
    SCREEN_MAIN_OFFLINE,   // 1. Ekran: İnternetsiz (Yerel Saat + BME280 Sensör)
    SCREEN_COMPASS         // 2. Ekran: Pusula Modu
} ScreenState_t;



void clock_init(void);
void bme_init(void);
void qmc_init(void);
void periph_init(void);
void app_init(void);



#endif