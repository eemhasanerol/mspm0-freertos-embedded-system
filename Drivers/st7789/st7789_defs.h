#ifndef ST7789_DEFS_H
#define ST7789_DEFS_H

#include <stdint.h>

#define GPIO_ST7789_DC_PORT 0
#define GPIO_ST7789_DC_PIN 0

#define ST7789_WIDTH   (240U)
#define ST7789_HEIGHT  (240U)

#define ST7789_CMD_NOP                (0x00U)   /**< no operation command */
#define ST7789_CMD_SWRESET            (0x01U)   /**< software reset command */

#define ST7789_CMD_RDDID              (0x04U)   
#define ST7789_CMD_RDDST              (0x09U)   
#define ST7789_CMD_RDDPM              (0x0AU)   
#define ST7789_CMD_RDDMADCTL          (0x0BU)
#define ST7789_CMD_RDDCOLMOD          (0x0CU)
#define ST7789_CMD_RDDIM              (0x0DU)
#define ST7789_CMD_RDDSM              (0x0EU)
#define ST7789_CMD_RDDSDR             (0x0FU)

#define ST7789_CMD_SLPIN              (0x10U)   /**< sleep in command */
#define ST7789_CMD_SLPOUT             (0x11U)   /**< sleep out command */
#define ST7789_CMD_PTLON              (0x12U)   /**< partial mode on command */
#define ST7789_CMD_NORON              (0x13U)   /**< normal display mode on command */

#define ST7789_CMD_INVOFF             (0x20U)   /**< display inversion off command */
#define ST7789_CMD_INVON              (0x21U)   /**< display inversion on command */
#define ST7789_CMD_GAMSET             (0x26U)   /**< display inversion set command */

#define ST7789_CMD_DISPOFF            (0x28U)   /**< display off command */
#define ST7789_CMD_DISPON             (0x29U)   /**< display on command */

#define ST7789_CMD_CASET              (0x2AU)   /**< column address set command */
#define ST7789_CMD_RASET              (0x2BU)   /**< row address set command */

#define ST7789_CMD_RAMWR              (0x2CU)    /**< memory write command */
#define ST7789_CMD_RAMRD              (0x2EU)

#define ST7789_CMD_PTLAR              (0x30U)    /**< partial start/end address set command */
#define ST7789_CMD_VSCRDEF            (0x33U)    /**< vertical scrolling definition command */

#define ST7789_CMD_TEOFF              (0x34U)   /**< tearing effect line off command */
#define ST7789_CMD_TEON               (0x35U)   /**< tearing effect line on command */       

#define ST7789_CMD_MADCTL             (0x36U)    /**< memory data access control command */
#define ST7789_CMD_VSCRSADD           (0x37U)   /**< vertical scrolling start address command */
 
#define ST7789_CMD_IDMOFF             (0x38U)   /**< idle mode off command */
#define ST7789_CMD_IDMON              (0x39U)   /**< idle mode on command */

#define ST7789_CMD_COLMOD             (0x3AU)   /**< interface pixel format command */

#define ST7789_CMD_RAMWRC             (0x3CU)   /**< memory write continue command */
#define ST7789_CMD_RAMRDC             (0x3EU)

#define ST7789_CMD_TESCAN             (0x44U)   /**< set tear scanline command */
#define ST7789_CMD_RDTESCAN           (0x45U)

#define ST7789_CMD_WRDISBV            (0x51U)    /**< write display brightness command */
#define ST7789_CMD_RDDISBV            (0x52U)

#define ST7789_CMD_WRCTRLD            (0x53U)   /**< write CTRL display command */
#define ST7789_CMD_RDCTRLD            (0x54U)

#define ST7789_CMD_WRCACE             (0x55U)   /**< write content adaptive brightness control and color enhancement command */
#define ST7789_CMD_RDCABC             (0x56U)   

#define ST7789_CMD_WRCABCMB           (0x5EU)   /**< write CABC minimum brightness command */
#define ST7789_CMD_RDCABCMB           (0x5FU)

#define ST7789_CMD_RDABCSDR           (0x68U)

#define ST7789_CMD_RDID1              (0xDAU)
#define ST7789_CMD_RDID2              (0xDBU)
#define ST7789_CMD_RDID3              (0xDCU)

#define ST7789_CMD_PORCTRL   (0xB2U)
#define ST7789_CMD_GCTRL     (0xB7U)
#define ST7789_CMD_VCOMS     (0xBBU)
#define ST7789_CMD_LCMCTRL   (0xC0U)
#define ST7789_CMD_VDVVRHEN  (0xC2U)
#define ST7789_CMD_VRHS      (0xC3U)
#define ST7789_CMD_VDVS      (0xC4U)
#define ST7789_CMD_FRCTRL2   (0xC6U)
#define ST7789_CMD_PWCTRL1   (0xD0U)

#define ST7789_COLOR_BLACK   0x0000
#define ST7789_COLOR_WHITE   0xFFFF
#define ST7789_COLOR_RED     0xF800
#define ST7789_COLOR_GREEN   0x07E0
#define ST7789_COLOR_BLUE    0x001F
#define ST7789_COLOR_YELLOW  0xFFE0
#define ST7789_COLOR_CYAN    0x07FF
#define ST7789_COLOR_GRID    0x52AA

#endif
