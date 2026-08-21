/**
  ******************************************************************************
  * @file    bme280.h
  * @author  Hasan Erol
  * @brief   High-level API for Bosch BME280 sensor
  ******************************************************************************
  * @attention
  * This file provides:
  *   - Data structures for device handle and sensor data
  *   - Function prototypes for initialization and configuration
  *   - Functions to read temperature, pressure, and humidity data
  *
  * For usage examples, see README.md
  ******************************************************************************
  */


#ifndef BME280_H_
#define BME280_H_

#include <stdint.h>
#include "bme280_defs.h"



/**
 * @brief Status codes for BME280 driver functions.
 *
 * These codes are returned by all public API functions
 * to indicate success or type of error.
 */
typedef enum {
    BME280_OK      = 0,   /*!< Operation successful */
    BME280_E_COMM  = -1,  /*!< Communication error (I2C/SPI) */
    BME280_E_ID    = -2,  /*!< Wrong chip ID */
    BME280_E_PARAM = -3   /*!< Invalid parameter */
} bme280_status_t;


/* -------------------------------------------------------------------------- */
/* Device configuration / handle                                              */
/* -------------------------------------------------------------------------- */
typedef struct {
    uint8_t dev_addr;  /*!< I2C address: @ref BME280_I2C_ADDR_SDO_LOW or
                            @ref BME280_I2C_ADDR_SDO_HIGH */

    uint8_t osr_t;     /*!< Temp oversampling.    @ref BME280_OSR_T  */
    uint8_t osr_p;     /*!< Pressure oversampling @ref BME280_OSR_P  */
    uint8_t osr_h;     /*!< Humidity oversampling @ref BME280_OSR_H  */
    uint8_t filter;    /*!< IIR filter.           @ref BME280_FILTER */
    uint8_t standby;   /*!< Standby time.         @ref BME280_STBY   */
    uint8_t mode;      /*!< Power mode.           @ref BME280_MODE   */

    /* I2C callbacks */
    int32_t (*i2c_read)(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len);
    int32_t (*i2c_write)(uint8_t dev, uint8_t reg, const uint8_t *buf, uint16_t len);
    void    (*delay_ms)(uint32_t ms);

    bme280_calib_data_t calib;
    int32_t t_fine;
} bme280_dev_t;

/* -------------------------------------------------------------------------- */
/* Raw & compensated data                                                     */
/* -------------------------------------------------------------------------- */
typedef struct {
    int32_t temp_raw;
    int32_t press_raw;
    int32_t hum_raw;
} bme280_raw_t;

typedef struct {
    float temperature_c;  /* °C */
    float pressure_pa;    /* Pa */
    float humidity_rh;    /* %RH */
} bme280_data_t;

/* -------------------------------------------------------------------------- */
/* API Prototypes                                                             */
/* -------------------------------------------------------------------------- */

bme280_status_t bme280_init(bme280_dev_t *dev);
bme280_status_t bme280_load_calibration(bme280_dev_t *dev);
bme280_status_t bme280_read_raw(bme280_dev_t *dev, bme280_raw_t *raw);

int32_t bme280_compensate_temperature(bme280_dev_t *dev, int32_t adc_T, int32_t *t_fine);
uint32_t bme280_compensate_pressure(bme280_dev_t *dev, int32_t adc_P, int32_t t_fine);
uint32_t bme280_compensate_humidity(bme280_dev_t *dev, int32_t adc_H, int32_t t_fine);

bme280_status_t bme280_read_all(bme280_dev_t *dev, bme280_data_t *data);
bme280_status_t bme280_read_temperature(bme280_dev_t *dev, bme280_data_t *data);
bme280_status_t bme280_read_pressure(bme280_dev_t *dev, bme280_data_t *data);
bme280_status_t bme280_read_humidity(bme280_dev_t *dev, bme280_data_t *data);

bme280_status_t bme280_read_id(bme280_dev_t *dev, uint8_t *chip_id);
bme280_status_t bme280_soft_reset(bme280_dev_t *dev);
bme280_status_t bme280_sleep(bme280_dev_t *dev);
bme280_status_t bme280_wakeup(bme280_dev_t *dev);

#endif /* BME280_INC_BME280_H_ */