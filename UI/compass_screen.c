#include "compass_screen.h"



static void UI_Compass_DrawDirections(void)
{
    int cx = 120;
    int cy = 120;

    struct
    {
        int deg;
        const char *txt;
    }
    dirs[] =
    {
        {0,   "N"},
        {90,  "E"},
        {180, "S"},
        {270, "W"},
    };

    for (int i = 0; i < 4; i++)
    {
        float rad =
            (dirs[i].deg - 90)
            * 3.14159265f / 180.0f;

        int x =
            cx + (int)(70 * cosf(rad));

        int y =
            cy + (int)(70 * sinf(rad));

        x -= Font_11x18.width / 2;
        y -= Font_11x18.height / 2;

        uint16_t txt_color;

        if (dirs[i].deg == 0)
        {
            txt_color = ST7789_COLOR_RED;
        }
        else
        {
            txt_color = ST7789_COLOR_WHITE;
        }

        ST7789_WriteString(
            x,
            y,
            dirs[i].txt,
            Font_11x18,
            txt_color,
            ST7789_COLOR_BLACK
        );
    }
}


static void UI_Compass_DrawMajorTicks(void)
{
    int cx = 120;
    int cy = 120;

    for (int deg = 0; deg < 360; deg += 30)
    {
        float rad =
            (deg - 90) * 3.14159265f / 180.0f;

        /* OUTER POINT */

        int x1 =
            cx + (int)(115 * cosf(rad));

        int y1 =
            cy + (int)(115 * sinf(rad));

        /* INNER POINT */

        int x2 =
            cx + (int)(97 * cosf(rad));

        int y2 =
            cy + (int)(97 * sinf(rad));

        /* THICK TICK */

        ST7789_DrawLine(
            x1,
            y1,
            x2,
            y2,
            ST7789_COLOR_WHITE
        );

        ST7789_DrawLine(
            x1 + 1,
            y1,
            x2 + 1,
            y2,
            ST7789_COLOR_WHITE
        );
    }
}

static void UI_Compass_DrawMinorTicks(void)
{
    int cx = 120;
    int cy = 120;

    for (int deg = 0; deg < 360; deg += 5)
    {
        /* SKIP MAJOR TICKS */

        if ((deg % 30) == 0)
        {
            continue;
        }

        float rad =
            (deg - 90) * 3.14159265f / 180.0f;

        /* OUTER POINT */

        int x1 =
            cx + (int)(115 * cosf(rad));

        int y1 =
            cy + (int)(115 * sinf(rad));

        /* INNER POINT */

        int x2 =
            cx + (int)(105 * cosf(rad));

        int y2 =
            cy + (int)(105 * sinf(rad));

        ST7789_DrawLine(
            x1,
            y1,
            x2,
            y2,
            ST7789_COLOR_WHITE
        );
    }
}



static void UI_Compass_DrawDegreeText(void)
{
    int cx = 120;
    int cy = 120;

    char str[8];

    for (int deg = 0; deg < 360; deg += 30)
    {
        float rad =
            (deg - 90) * 3.14159265f / 180.0f;

        int x =
            cx + (int)(92 * cosf(rad));

        int y =
            cy + (int)(92 * sinf(rad));

        snprintf(str, sizeof(str), "%d", deg);

    snprintf(str, sizeof(str), "%d", deg);

    int len = strlen(str);

    x -= (len * Font_7x10.width) / 2;
    y -= Font_7x10.height / 2;

    if (deg == 0)
    {
        y += 3;
        x += 1;
    }
            ST7789_WriteString(
            x,
            y,
            str,
            Font_7x10,
            ST7789_COLOR_WHITE,
            ST7789_COLOR_BLACK
        );
    }
}


void UI_CompassScreen_DrawNeedle(float angle, uint16_t color)
{
    int cx = 120;
    int cy = 120;

    int len = 60;

    float rad =
        (angle - 90.0f)
        * 3.14159265f / 180.0f;

    int x2 =
        cx + (int)(len * cosf(rad));

    int y2 =
        cy + (int)(len * sinf(rad));

    /* MAIN THIN LINE */

    ST7789_DrawLine(
        cx,
        cy,
        x2,
        y2,
        color
    );

    /* ARROW HEAD */

    int arrow = 10;

    float a1 = rad + 2.5f;
    float a2 = rad - 2.5f;

    int lx =
        x2 + (int)(arrow * cosf(a1));

    int ly =
        y2 + (int)(arrow * sinf(a1));

    int rx =
        x2 + (int)(arrow * cosf(a2));

    int ry =
        y2 + (int)(arrow * sinf(a2));

    ST7789_DrawLine(
        x2,
        y2,
        lx,
        ly,
        color
    );

    ST7789_DrawLine(
        x2,
        y2,
        rx,
        ry,
        color
    );
}

void UI_CompassScreen_DrawBackground(void)
{
    // Ekranı siyaha boya
    ST7789_FillScreen(ST7789_COLOR_BLACK);           
    
    // Pusula dış halkasını çiz (3 piksel kalınlığında)
    for (int r = 120; r >= 118; r--)
    {
        ST7789_DrawCircle(120, 120, r, ST7789_COLOR_WHITE);
    }

    // Pusula sabit detaylarını yerleştir
    UI_Compass_DrawDirections(); 
    UI_Compass_DrawMajorTicks();
    UI_Compass_DrawMinorTicks();
    UI_Compass_DrawDegreeText();
}

