#ifndef PLATFORM_H
#define PLATFORM_H

#include "ti_msp_dl_config.h"
#include "stdint.h"

// FreeRTOS Çekirdek Kütüphaneleri
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"


extern SemaphoreHandle_t xI2CMutex;

int32_t platform_i2c_read(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len);
int32_t platform_i2c_write(uint8_t dev, uint8_t reg, const uint8_t *buf, uint16_t len);
void platform_delay_ms(uint32_t ms);


#endif