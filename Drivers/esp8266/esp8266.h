#ifndef ESP8266_H_
#define ESP8266_H_

#include "ti_msp_dl_config.h"
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "st7789.h"

// FreeRTOS Çekirdek Kütüphaneleri
#include "FreeRTOS.h"
#include "task.h"



extern char ev_wifi_adi[32];
extern char ev_wifi_sifresi[32];

char response_buffer[120];


void UART_writeString(const char *str);
void delay_ms(uint32_t ms);
bool wifi_read(void);
bool esp_wifi_kur_ve_baglan(void);
void Save_WiFi_To_Flash(const char* ssid, const char* pass);
void Load_WiFi_From_Flash(char* output_buffer);
bool esp_wifi_durum_kontrol(void);


void esp8266_hw_reset(void);

#endif /* ESP8266_H_ */