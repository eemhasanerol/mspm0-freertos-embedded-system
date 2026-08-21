#ifndef OFFLINE_SCREEN_H_
#define OFFLINE_SCREEN_H_


#include <stdio.h>
#include <string.h>
#include "st7789.h"
#include "bme280.h"
#include "ds1307.h"



void UI_OfflineScreen_DrawBackground(void);
void UI_OfflineScreen_UpdateBME280(bme280_dev_t *dev);
void UI_OfflineScreen_UpdateClock(ds1307_dev_t *dev);



#endif /* OFFLINE_SCREEN_H_ */