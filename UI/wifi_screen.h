#ifndef WIFI_SCREEN_H_
#define WIFI_SCREEN_H_
#include "ds1307.h"
#include "st7789.h"
#include "esp8266.h"


void UI_WifiScreen_DrawBackground(void);
void UI_WifiScreen_UpdateClock(ds1307_dev_t *dev);
void UI_WifiScreen_Update(void);



#endif 