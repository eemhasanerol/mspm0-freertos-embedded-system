/**
  ******************************************************************************
  * @file    ds1307_defs.h
  * @author  Hasan Erol
  * @brief   Register definitions and configuration macros for DS1307 RTC
  ******************************************************************************
  * @attention
  * Reference: DS1307 Datasheet
  *
  * This file provides register addresses, bit masks, and helper definitions
  * for the DS1307 real-time clock.
  ******************************************************************************
  */

#ifndef DS1307_DEFS_H_
#define DS1307_DEFS_H_

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Default I2C address                                                        */
/* -------------------------------------------------------------------------- */
#define DS1307_I2C_ADDR          (0x68U)

/* -------------------------------------------------------------------------- */
/* Register map                                                               */
/* -------------------------------------------------------------------------- */
#define DS1307_REG_SECONDS       (0x00U)
#define DS1307_REG_MINUTES       (0x01U)
#define DS1307_REG_HOURS         (0x02U)
#define DS1307_REG_DAY           (0x03U)
#define DS1307_REG_DATE          (0x04U)
#define DS1307_REG_MONTH         (0x05U)
#define DS1307_REG_YEAR          (0x06U)
#define DS1307_REG_CONTROL       (0x07U)
#define DS1307_REG_RAM_START     (0x08U)
#define DS1307_REG_RAM_END       (0x3FU)

/* -------------------------------------------------------------------------- */
/* Day of week                                                                */
/* -------------------------------------------------------------------------- */
/** @defgroup DS1307_DayOfWeek Day of week values
 *  @brief Day of week definitions (1–7)
 *  @{
 */
typedef enum
{
    DS1307_SUNDAY    = 1U,
    DS1307_MONDAY    = 2U,
    DS1307_TUESDAY   = 3U,
    DS1307_WEDNESDAY = 4U,
    DS1307_THURSDAY  = 5U,
    DS1307_FRIDAY    = 6U,
    DS1307_SATURDAY  = 7U
} ds1307_day_of_week_t;
/** @} */

/* -------------------------------------------------------------------------- */
/* Bit masks                                                                  */
/* -------------------------------------------------------------------------- */
/** @defgroup DS1307_Bits Key bit masks
 *  @brief Bit mask definitions for control and time registers
 *  @{
 */
#define DS1307_SECONDS_CH_MASK   (1U << 7)  /*!< Clock Halt bit */

/* Hours register (0x02) */
#define DS1307_HOUR_24H          (0U << 6U) /*!< 0 = 24h mode */
#define DS1307_HOUR_12H          (1U << 6U) /*!< 1 = 12h mode */
#define DS1307_HOUR_AMPM         (1U << 5U) /*!< 0 = AM, 1 = PM (12h mode) */

/* Control register (0x07) */
#define DS1307_CTRL_OUT          (1U << 7)  /*!< Output control */
#define DS1307_CTRL_SQWE         (1U << 4)  /*!< Square-wave enable */
#define DS1307_CTRL_RS1          (1U << 1)  /*!< Rate select 1 */
#define DS1307_CTRL_RS0          (1U << 0)  /*!< Rate select 0 */

/* Meridiem helper values */
#define DS1307_AM                (0U)
#define DS1307_PM                (1U)
/** @} */

#endif /* DS1307_INC_DS1307_DEFS_H_ */