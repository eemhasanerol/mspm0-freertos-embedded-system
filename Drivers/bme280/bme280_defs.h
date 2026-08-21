/**
  ******************************************************************************
  * @file    bme280_defs.h
  * @author  Hasan Erol
  * @brief   Register map and option macros for Bosch BME280 sensor
  ******************************************************************************
  * @attention
  * Reference: Bosch BME280 Datasheet, Rev 1.6
  *
  * This file provides all register addresses, bit masks, and configuration
  * options for the BME280 temperature, pressure, and humidity sensor.
  ******************************************************************************
  */


#ifndef INC_BME280_DEFS_H_
#define INC_BME280_DEFS_H_

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Register Map                                                               */
/* -------------------------------------------------------------------------- */
#define BME280_REG_ID           (0xD0)  /*!< Chip ID register */
#define BME280_REG_RESET        (0xE0)  /*!< Soft reset register */
#define BME280_REG_CTRL_HUM     (0xF2)  /*!< Humidity oversampling */
#define BME280_REG_STATUS       (0xF3)  /*!< Status flags */
#define BME280_REG_CTRL_MEAS    (0xF4)  /*!< Temp/Press oversampling + mode */
#define BME280_REG_CONFIG       (0xF5)  /*!< IIR filter + standby */
#define BME280_REG_PRESS_MSB    (0xF7)
#define BME280_REG_PRESS_LSB    (0xF8)
#define BME280_REG_PRESS_XLSB   (0xF9)
#define BME280_REG_TEMP_MSB     (0xFA)
#define BME280_REG_TEMP_LSB     (0xFB)
#define BME280_REG_TEMP_XLSB    (0xFC)
#define BME280_REG_HUM_MSB      (0xFD)
#define BME280_REG_HUM_LSB      (0xFE)

/* Fixed values */
#define BME280_CHIP_ID          (0x60)  /*!< Expected chip ID */
#define BME280_RESET_VALUE      (0xB6)  /*!< Soft reset command */

/* -------------------------------------------------------------------------- */
/* I2C addresses (depends on SDO pin state)                                   */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_I2C_ADDR I2C device addresses
 *  @brief I2C address options depending on SDO pin
 *  @{
 */
#define BME280_I2C_ADDR_SDO_LOW   (0x76U)  /*!< SDO = 0 -> I2C address 0x76 */
#define BME280_I2C_ADDR_SDO_HIGH  (0x77U)  /*!< SDO = 1 -> I2C address 0x77 */
/** @} */


/* -------------------------------------------------------------------------- */
/* Power modes                                                                */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_MODE Power modes
 *  @brief Selectable power modes for the BME280
 *  @{
 */
#define BME280_MODE_SLEEP       (0x00 << 0)  /*!< Sleep mode */
#define BME280_MODE_FORCED      (0x01 << 0)  /*!< Forced mode */
#define BME280_MODE_NORMAL      (0x03 << 0)  /*!< Normal mode */
/** @} */


/* -------------------------------------------------------------------------- */
/* Temperature oversampling                                                   */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_OSR_T Temperature oversampling
 *  @brief Oversampling settings for temperature
 *  @{
 */
#define BME280_OSR_T_SKIP       (0x00 << 5)  /*!< Skip temperature */
#define BME280_OSR_T_1X         (0x01 << 5)
#define BME280_OSR_T_2X         (0x02 << 5)
#define BME280_OSR_T_4X         (0x03 << 5)
#define BME280_OSR_T_8X         (0x04 << 5)
#define BME280_OSR_T_16X        (0x05 << 5)
/** @} */


/* -------------------------------------------------------------------------- */
/* Pressure oversampling                                                      */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_OSR_P Pressure oversampling
 *  @brief Oversampling settings for pressure
 *  @{
 */
#define BME280_OSR_P_SKIP       (0x00 << 2)  /*!< Skip pressure */
#define BME280_OSR_P_1X         (0x01 << 2)
#define BME280_OSR_P_2X         (0x02 << 2)
#define BME280_OSR_P_4X         (0x03 << 2)
#define BME280_OSR_P_8X         (0x04 << 2)
#define BME280_OSR_P_16X        (0x05 << 2)
/** @} */


/* -------------------------------------------------------------------------- */
/* Humidity oversampling                                                      */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_OSR_H Humidity oversampling
 *  @brief Oversampling settings for humidity
 *  @{
 */
#define BME280_OSR_H_SKIP       (0x00)
#define BME280_OSR_H_1X         (0x01)
#define BME280_OSR_H_2X         (0x02)
#define BME280_OSR_H_4X         (0x03)
#define BME280_OSR_H_8X         (0x04)
#define BME280_OSR_H_16X        (0x05)
/** @} */


/* -------------------------------------------------------------------------- */
/* IIR filter                                                                 */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_FILTER IIR filter settings
 *  @brief Selectable IIR filter coefficients
 *  @{
 */
#define BME280_FILTER_OFF       (0x00 << 2)
#define BME280_FILTER_2         (0x01 << 2)
#define BME280_FILTER_4         (0x02 << 2)
#define BME280_FILTER_8         (0x03 << 2)
#define BME280_FILTER_16        (0x04 << 2)
/** @} */


/* -------------------------------------------------------------------------- */
/* Standby time                                                               */
/* -------------------------------------------------------------------------- */
/** @defgroup BME280_STBY Standby time
 *  @brief Standby durations in normal mode
 *  @{
 */
#define BME280_STBY_0_5_MS      (0x00 << 5)
#define BME280_STBY_62_5_MS     (0x01 << 5)
#define BME280_STBY_125_MS      (0x02 << 5)
#define BME280_STBY_250_MS      (0x03 << 5)
#define BME280_STBY_500_MS      (0x04 << 5)
#define BME280_STBY_1000_MS     (0x05 << 5)
#define BME280_STBY_10_MS       (0x06 << 5)
#define BME280_STBY_20_MS       (0x07 << 5)
/** @} */


/* -------------------------------------------------------------------------- */
/* Calibration data                                                           */
/* -------------------------------------------------------------------------- */
/**
 * @brief Calibration parameters structure.
 * Fields are read from NVM registers 0x88–0xA1 and 0xE1–0xE7.
 */
typedef struct
{
    uint16_t  dig_T1;
    int16_t   dig_T2;
    int16_t   dig_T3;

    uint16_t  dig_P1;
    int16_t   dig_P2;
    int16_t   dig_P3;
    int16_t   dig_P4;
    int16_t   dig_P5;
    int16_t   dig_P6;
    int16_t   dig_P7;
    int16_t   dig_P8;
    int16_t   dig_P9;

    uint8_t   dig_H1;
    int16_t   dig_H2;
    uint8_t   dig_H3;
    int16_t   dig_H4;
    int16_t   dig_H5;
    int8_t    dig_H6;

} bme280_calib_data_t;

#endif /* INC_BME280_DEFS_H_ */