#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ti_msp_dl_config.h"
#include "st7789.h"

void ST7789_DelayMs(uint32_t ms)
{
    // Kör döngüyü çöpe atıyoruz abicim
    // delay_cycles(ms * 32000); 
    
    // Yerine FreeRTOS uykusunu çakıyoruz. İşlemci boşa çıksın, diğer tasklar çalışsın mk!
delay_cycles(ms * 32000);
}

static void ST7789_Write8(uint8_t d)
{
    while (DL_SPI_isTXFIFOFull(SPI_INST));

    DL_SPI_transmitData8(SPI_INST, d);

    while (DL_SPI_isBusy(SPI_INST));
}

static void ST7789_Cmd(uint8_t c)
{
    DL_GPIO_clearPins(ST7789_PORT, ST7789_DC_PIN);
    ST7789_Write8(c);
}

static void ST7789_Data8(uint8_t d)
{
    DL_GPIO_setPins(ST7789_PORT, ST7789_DC_PIN);
    ST7789_Write8(d);
}

void ST7789_Init(void)
{
    DL_GPIO_setPins(ST7789_PORT, ST7789_BLK_PIN);

    DL_GPIO_clearPins(ST7789_PORT, ST7789_RESET_PIN);
    ST7789_DelayMs(100);
    DL_GPIO_setPins(ST7789_PORT, ST7789_RESET_PIN);
    ST7789_DelayMs(200);

    ST7789_Cmd(ST7789_CMD_SWRESET);
    ST7789_DelayMs(200);

    ST7789_Cmd(ST7789_CMD_SLPOUT);
    ST7789_DelayMs(200);

    ST7789_Cmd(ST7789_CMD_COLMOD);
    ST7789_Data8(0x55);

    ST7789_Cmd(ST7789_CMD_MADCTL);
    ST7789_Data8(0x00);

    ST7789_Cmd(ST7789_CMD_PORCTRL);
    ST7789_Data8(0x0C);
    ST7789_Data8(0x0C);
    ST7789_Data8(0x00);
    ST7789_Data8(0x33);
    ST7789_Data8(0x33);

    ST7789_Cmd(ST7789_CMD_GCTRL);
    ST7789_Data8(0x35);

    ST7789_Cmd(ST7789_CMD_VCOMS);
    ST7789_Data8(0x19);

    ST7789_Cmd(ST7789_CMD_LCMCTRL);
    ST7789_Data8(0x2C);

    ST7789_Cmd(ST7789_CMD_VDVVRHEN);
    ST7789_Data8(0x01);

    ST7789_Cmd(ST7789_CMD_VRHS);
    ST7789_Data8(0x12);

    ST7789_Cmd(ST7789_CMD_VDVS);
    ST7789_Data8(0x20);

    ST7789_Cmd(ST7789_CMD_FRCTRL2);
    ST7789_Data8(0x0F);

    ST7789_Cmd(ST7789_CMD_PWCTRL1);
    ST7789_Data8(0xA4);
    ST7789_Data8(0xA1);

    ST7789_Cmd(ST7789_CMD_INVON);
    ST7789_DelayMs(10);

    ST7789_Cmd(ST7789_CMD_DISPON);
    ST7789_DelayMs(800);
}

void ST7789_DrawRect(uint16_t x,
                     uint16_t y,
                     uint16_t w,
                     uint16_t h,
                     uint16_t color)
{
    if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT) {
        return;
    }

    if ((x + w) > ST7789_WIDTH) {
        w = ST7789_WIDTH - x;
    }

    if ((y + h) > ST7789_HEIGHT) {
        h = ST7789_HEIGHT - y;
    }

    ST7789_Cmd(ST7789_CMD_CASET);
    ST7789_Data8(x >> 8);
    ST7789_Data8(x & 0xFF);
    ST7789_Data8((x + w - 1) >> 8);
    ST7789_Data8((x + w - 1) & 0xFF);

    ST7789_Cmd(ST7789_CMD_RASET);
    ST7789_Data8(y >> 8);
    ST7789_Data8(y & 0xFF);
    ST7789_Data8((y + h - 1) >> 8);
    ST7789_Data8((y + h - 1) & 0xFF);

    ST7789_Cmd(ST7789_CMD_RAMWR);

    DL_GPIO_setPins(ST7789_PORT, ST7789_DC_PIN);

    for (uint32_t i = 0; i < (uint32_t)w * h; i++) {
        ST7789_Write8(color >> 8);
        ST7789_Write8(color & 0xFF);
    }
}

void ST7789_FillScreen(uint16_t color)
{
    ST7789_DrawRect(0, 0, ST7789_WIDTH, ST7789_HEIGHT, color);
}

void ST7789_DrawPixel(uint16_t x, uint16_t y, uint16_t color)
{
    if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT) {
        return;
    }

    ST7789_DrawRect(x, y, 1, 1, color);
}

void ST7789_WriteChar(uint16_t x,
                      uint16_t y,
                      char ch,
                      FontDef font,
                      uint16_t color,
                      uint16_t bgcolor)
{
    uint16_t line;

    if (x >= ST7789_WIDTH || y >= ST7789_HEIGHT) {
        return;
    }

    if (ch < 32 || ch > 126) {
        ch = '?';
    }

    for (uint32_t row = 0; row < font.height; row++) {
        line = font.data[(ch - 32) * font.height + row];

        for (uint32_t col = 0; col < font.width; col++) {
            if (line & (0x8000 >> col)) {
                ST7789_DrawPixel(x + col, y + row, color);
            } else {
                ST7789_DrawPixel(x + col, y + row, bgcolor);
            }
        }
    }
}

void ST7789_WriteString(uint16_t x,
                        uint16_t y,
                        const char *str,
                        FontDef font,
                        uint16_t color,
                        uint16_t bgcolor)
{
    while (*str) {
        if ((x + font.width) >= ST7789_WIDTH) {
            x = 0;
            y += font.height;
        }

        if ((y + font.height) >= ST7789_HEIGHT) {
            break;
        }

        ST7789_WriteChar(x, y, *str, font, color, bgcolor);

        x += font.width + 1;
        str++;
    }
}

void ST7789_DrawFilledCircle(int x0, int y0, int r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        // 1. Üst ve alt yarıdaki geniş yatay çizgileri çekiyoruz mk
        // (x0 - x) noktasından (x0 + x) noktasına, y0 + y hizasında dümdüz çizgi
        ST7789_DrawLine(x0 - x, y0 + y, x0 + x, y0 + y, color);
        ST7789_DrawLine(x0 - x, y0 - y, x0 + x, y0 - y, color);

        // 2. Orta kısımdaki dikey olarak daha geniş olan yatay çizgileri çekiyoruz
        // (x0 - y) noktasından (x0 + y) noktasına, y0 + x hizasında dümdüz çizgi
        ST7789_DrawLine(x0 - y, y0 + x, x0 + y, y0 + x, color);
        ST7789_DrawLine(x0 - y, y0 - x, x0 + y, y0 - x, color);

        if (err <= 0)
        {
            y++;
            err += 2 * y + 1;
        }

        if (err > 0)
        {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void ST7789_DrawCircle(int x0, int y0, int r, uint16_t color)
{
    int x = r;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        ST7789_DrawPixel(x0 + x, y0 + y, color);
        ST7789_DrawPixel(x0 + y, y0 + x, color);

        ST7789_DrawPixel(x0 - y, y0 + x, color);
        ST7789_DrawPixel(x0 - x, y0 + y, color);

        ST7789_DrawPixel(x0 - x, y0 - y, color);
        ST7789_DrawPixel(x0 - y, y0 - x, color);

        ST7789_DrawPixel(x0 + y, y0 - x, color);
        ST7789_DrawPixel(x0 + x, y0 - y, color);

        if (err <= 0)
        {
            y++;
            err += 2 * y + 1;
        }

        if (err > 0)
        {
            x--;
            err -= 2 * x + 1;
        }
    }
}

void ST7789_DrawLine(int x0,
                     int y0,
                     int x1,
                     int y1,
                     uint16_t color)
{
    int dx = abs(x1 - x0);
    int sx = x0 < x1 ? 1 : -1;

    int dy = -abs(y1 - y0);
    int sy = y0 < y1 ? 1 : -1;

    int err = dx + dy;

    while (1)
    {
        ST7789_DrawPixel(x0, y0, color);

        if (x0 == x1 && y0 == y1)
        {
            break;
        }

        int e2 = 2 * err;

        if (e2 >= dy)
        {
            err += dy;
            x0 += sx;
        }

        if (e2 <= dx)
        {
            err += dx;
            y0 += sy;
        }
    }
}



void ST7789_DrawChar_Scaled(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color) {
    // Karakterin başlangıç indeksini hesapla (Yükseklik 18 olduğu için)
    uint16_t char_index = (c - ' ') * 18; 
    
    for (uint16_t row = 0; row < 18; row++) {
        // Struct üzerinden asıl diziye erişiyoruz
        uint16_t row_data = Font_11x18.data[char_index + row];
        
        for (uint16_t col = 0; col < 11; col++) {
            // X ve Y koordinatlarının 2 katını hesaplıyoruz (col * 2 ve row * 2)
            uint16_t x_pos = x + (col << 1);
            uint16_t y_pos = y + (row << 1);
            
            if (row_data & (0x8000 >> col)) {
                // Bit 1 ise: 2x2'lik pikselleri yazı rengiyle doldur
                ST7789_DrawPixel(x_pos,     y_pos,     color);
                ST7789_DrawPixel(x_pos + 1, y_pos,     color);
                ST7789_DrawPixel(x_pos,     y_pos + 1, color);
                ST7789_DrawPixel(x_pos + 1, y_pos + 1, color);
            } else {
                // Bit 0 ise: 2x2'lik pikselleri arka plan rengiyle doldur (Temizleme)
                ST7789_DrawPixel(x_pos,     y_pos,     bg_color);
                ST7789_DrawPixel(x_pos + 1, y_pos,     bg_color);
                ST7789_DrawPixel(x_pos,     y_pos + 1, bg_color);
                ST7789_DrawPixel(x_pos + 1, y_pos + 1, bg_color);
            }
        }
    }
}


void ST7789_DrawChar_Font16x26_Scaled2x(uint16_t x, uint16_t y, char c, uint16_t color, uint16_t bg_color) {
    // Karakterin başlangıç indeksini Font_16x26 yüksekliğine (26) göre hesapla
    uint16_t char_index = (c - ' ') * 26; 
    
    for (uint16_t row = 0; row < 26; row++) {
        // Struct üzerinden 16x26 fontunun asıl dizisine erişiyoruz
        uint16_t row_data = Font_16x26.data[char_index + row];
        
        for (uint16_t col = 0; col < 16; col++) {
            // Her pikselin 2 katı büyüklükteki yeni koordinatlarını hesapla
            uint16_t x_pos = x + (col << 1);
            uint16_t y_pos = y + (row << 1);
            
            if (row_data & (0x8000 >> col)) {
                // Bit 1 ise: 2x2 piksellik alanı yazı rengiyle doldur
                ST7789_DrawPixel(x_pos,     y_pos,     color);
                ST7789_DrawPixel(x_pos + 1, y_pos,     color);
                ST7789_DrawPixel(x_pos,     y_pos + 1, color);
                ST7789_DrawPixel(x_pos + 1, y_pos + 1, color);
            } else {
                // Bit 0 ise: 2x2 piksellik alanı arka plan rengiyle doldur
                ST7789_DrawPixel(x_pos,     y_pos,     bg_color);
                ST7789_DrawPixel(x_pos + 1, y_pos,     bg_color);
                ST7789_DrawPixel(x_pos,     y_pos + 1, bg_color);
                ST7789_DrawPixel(x_pos + 1, y_pos + 1, bg_color);
            }
        }
    }
}
