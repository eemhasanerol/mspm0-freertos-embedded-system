#ifndef ST7789_H_
#define ST7789_H_

#include <stdint.h>
#include "st7789_defs.h"
#include "st7789_fonts.h"

void ST7789_Init(void);

void ST7789_FillScreen(uint16_t color);
void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color);
void ST7789_DrawRect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t color);

void ST7789_WriteChar(uint16_t x,
                      uint16_t y,
                      char ch,
                      FontDef font,
                      uint16_t color,
                      uint16_t bgcolor);

void ST7789_WriteString(uint16_t x,
                        uint16_t y,
                        const char *str,
                        FontDef font,
                        uint16_t color,
                        uint16_t bgcolor);

void ST7789_DrawCircle(int x0, int y0, int r, uint16_t color);



void ST7789_DrawLine(int x0,
                     int y0,
                     int x1,
                     int y1,
                     uint16_t color);

void ST7789_DrawFilledCircle(int x0, int y0, int r, uint16_t color);



void ST7789_DrawChar_Scaled(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color);
void ST7789_DrawChar_Font16x26_Scaled2x(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color);

#endif