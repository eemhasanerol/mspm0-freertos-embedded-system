/**
  ******************************************************************************
  * @file    ds1307.h
  * @author  erol-tesla
  * @brief   Public API for DS1307 RTC driver
  ******************************************************************************
  * @attention
  * Reference: DS1307 Datasheet
  *
  * This file provides:
  *   - Device handle and time/date structure
  *   - Status codes for driver functions
  *   - Function prototypes for initialization and time read
  ******************************************************************************
  */

#ifndef DS1307_H_
#define DS1307_H_

#include <stdint.h>
#include "ds1307_defs.h"


/**
 * @brief Status codes for DS1307 driver functions.
 */
typedef enum {
    DS1307_OK      = 0,   /*!< Operation successful */
    DS1307_E_COMM  = -1,  /*!< Communication error (I2C) */
    DS1307_E_PARAM = -2   /*!< Invalid parameter */
} ds1307_status_t;


/* -------------------------------------------------------------------------- */
/* Time/Date Structure                                                        */
/* -------------------------------------------------------------------------- */
typedef struct
{
    uint8_t seconds;      /*!< 0-59 */
    uint8_t minutes;      /*!< 0-59 */
    uint8_t hours;        /*!< 0-23 or 1-12 (mode-dependent) */

    uint8_t day_of_week;  /*!< 1-7 (Sunday=1 ... Saturday=7) */
    uint8_t date;         /*!< 1-31 */
    uint8_t month;        /*!< 1-12 */
    uint8_t year;         /*!< 0-99 (represents 2000+year) */

    uint8_t time_format;  /*!< 0:24h, 1:12h */
    uint8_t meridiem;     /*!< 0:AM, 1:PM (valid if 12h mode) */
} ds1307_time_t;

/* -------------------------------------------------------------------------- */
/* Device Handle                                                              */
/* -------------------------------------------------------------------------- */
typedef struct
{
    uint8_t dev_addr;   /*!< Default: DS1307_I2C_ADDR */

    /* I2C hooks (must be set by user) */
    int32_t (*i2c_read)(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len);
    int32_t (*i2c_write)(uint8_t dev, uint8_t reg, const uint8_t *buf, uint16_t len);

    ds1307_time_t time; /*!< Current time/date container */
} ds1307_dev_t;

/* -------------------------------------------------------------------------- */
/* BCD Helpers                                                                */
/* -------------------------------------------------------------------------- */
/**
 * @brief Convert BCD to binary.
 */
static inline uint8_t ds1307_bcd_to_bin(uint8_t x)
{
    return (uint8_t)(((x >> 4) * 10u) + (x & 0x0Fu));
}

/**
 * @brief Convert binary to BCD.
 */
static inline uint8_t ds1307_bin_to_bcd(uint8_t x)
{
    return (uint8_t)(((x / 10u) << 4) | (x % 10u));
}

/* -------------------------------------------------------------------------- */
/* Public API                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize DS1307 device.
 *
 * Writes initial time/date from @ref ds1307_time_t into the device.
 * Also ensures that the device handle has a valid I2C address and hooks.
 *
 * @param[in] dev Pointer to DS1307 device handle
 * @return ds1307_status_t
 */
ds1307_status_t ds1307_init(ds1307_dev_t *dev);

/**
 * @brief Read current time/date from DS1307.
 *
 * Reads registers 0x00–0x06, converts them from BCD to binary,
 * and fills @ref ds1307_time_t in the device handle.
 *
 * @param[in,out] dev Pointer to DS1307 device handle
 * @return ds1307_status_t
 */
ds1307_status_t ds1307_get_time(ds1307_dev_t *dev);


/* Yardımcı isim fonksiyonlarının tanımları */
const char* GetDayName(uint8_t day);
const char* GetMonthName(uint8_t month);


#endif /* DS1307_INC_DS1307_H_ */