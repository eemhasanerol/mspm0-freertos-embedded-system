/**
  ******************************************************************************
  * @file    bme280.c
  * @author  Hasan Erol
  * @brief   Source file for Bosch BME280 driver
  ******************************************************************************
  * @attention
  * This file contains the implementation of initialization, calibration,
  * compensation, and data readout functions for the BME280 sensor.
  ******************************************************************************
  */

#include "bme280.h"


/**
 * @brief Initialize BME280 (reset, config, oversampling, filter).
 * @param[in] dev  Device handle
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_init(bme280_dev_t *dev)
{
    if (!dev || !dev->i2c_read || !dev->i2c_write) {
        return BME280_E_PARAM;  /* Null pointer or invalid handle */
    }

    uint8_t chip_id = 0;

    /* 1. Read Chip ID */
    if (dev->i2c_read(dev->dev_addr, BME280_REG_ID, &chip_id, 1U) != 0) {
        return BME280_E_COMM;  /* Communication error */
    }
    if (chip_id != BME280_CHIP_ID) {
        return BME280_E_ID;    /* Wrong device ID */
    }

    /* 2. Soft reset */
    uint8_t reset_cmd = BME280_RESET_VALUE;
    if (dev->i2c_write(dev->dev_addr, BME280_REG_RESET, &reset_cmd, 1U) != 0) {
        return BME280_E_COMM;
    }
    if (dev->delay_ms) {
        dev->delay_ms(2U);
    }

    /* 3. Humidity oversampling */
    if (dev->i2c_write(dev->dev_addr, BME280_REG_CTRL_HUM, &dev->osr_h, 1U) != 0) {
        return BME280_E_COMM;
    }

    /* 4. Filter + standby */
    uint8_t config = (uint8_t)(dev->filter | dev->standby);
    if (dev->i2c_write(dev->dev_addr, BME280_REG_CONFIG, &config, 1U) != 0) {
        return BME280_E_COMM;
    }

    /* 5. Temp + press oversampling + mode */
    uint8_t ctrl_meas = (uint8_t)(dev->osr_t | dev->osr_p | dev->mode);
    if (dev->i2c_write(dev->dev_addr, BME280_REG_CTRL_MEAS, &ctrl_meas, 1U) != 0) {
        return BME280_E_COMM;
    }

    dev->delay_ms(5);

    /* 6. Load calibration data */
    if (bme280_load_calibration(dev) != BME280_OK) {
        return BME280_E_COMM;
    }

    return BME280_OK;
}



/**
 * @brief Load calibration coefficients into dev->calib.
 * @param[in] dev  Device handle
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code) 
 */
bme280_status_t bme280_load_calibration(bme280_dev_t *dev)
{
    if (!dev) {
        return BME280_E_PARAM;   /* null pointer check */
    }

    uint8_t buf24[24];
    uint8_t h1;
    uint8_t bufE1[7];

    /* 0x88'dan 24 byte: T1..T3, P1..P9 */
    if (dev->i2c_read(dev->dev_addr, 0x88, buf24, 24U) != 0) {
        return BME280_E_COMM;
    }

    dev->calib.dig_T1 = (uint16_t)((buf24[1] << 8) | buf24[0]);
    dev->calib.dig_T2 = (int16_t)((buf24[3] << 8) | buf24[2]);
    dev->calib.dig_T3 = (int16_t)((buf24[5] << 8) | buf24[4]);

    dev->calib.dig_P1 = (uint16_t)((buf24[7] << 8) | buf24[6]);
    dev->calib.dig_P2 = (int16_t)((buf24[9] << 8) | buf24[8]);
    dev->calib.dig_P3 = (int16_t)((buf24[11] << 8) | buf24[10]);
    dev->calib.dig_P4 = (int16_t)((buf24[13] << 8) | buf24[12]);
    dev->calib.dig_P5 = (int16_t)((buf24[15] << 8) | buf24[14]);
    dev->calib.dig_P6 = (int16_t)((buf24[17] << 8) | buf24[16]);
    dev->calib.dig_P7 = (int16_t)((buf24[19] << 8) | buf24[18]);
    dev->calib.dig_P8 = (int16_t)((buf24[21] << 8) | buf24[20]);
    dev->calib.dig_P9 = (int16_t)((buf24[23] << 8) | buf24[22]);

    /* 0xA1: H1 */
    if (dev->i2c_read(dev->dev_addr, 0xA1, &h1, 1U) != 0) {
        return BME280_E_COMM;
    }
    dev->calib.dig_H1 = h1;

    /* 0xE1'den 7 byte: H2..H6 (bit paketli) */
    if (dev->i2c_read(dev->dev_addr, 0xE1, bufE1, 7U) != 0) {
        return BME280_E_COMM;
    }

    dev->calib.dig_H2 = (int16_t)((bufE1[1] << 8) | bufE1[0]);
    dev->calib.dig_H3 = bufE1[2];
    dev->calib.dig_H4 = (int16_t)(((int16_t)bufE1[3] << 4) | (bufE1[4] & 0x0F));
    dev->calib.dig_H5 = (int16_t)(((int16_t)bufE1[5] << 4) | (bufE1[4] >> 4));
    dev->calib.dig_H6 = (int8_t)bufE1[6];

    return BME280_OK;
}

/**
 * @brief Read raw uncompensated sensor values.
 * @param[in]  dev  Device handle
 * @param[out] raw  Raw data struct
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_read_raw(bme280_dev_t *dev, bme280_raw_t *raw)
{
    if (!dev || !raw) {
        return BME280_E_PARAM;
    }

    uint8_t buf[8];

    /* Read pressure(3) + temperature(3) + humidity(2) = 8 bytes */
    if (dev->i2c_read(dev->dev_addr, BME280_REG_PRESS_MSB, buf, 8) != 0) {
        return BME280_E_COMM;
    }

    raw->press_raw = (((int32_t)buf[0] << 12U) |
                      ((int32_t)buf[1] << 4U) |
                      ((int32_t)buf[2] >> 4U));

    raw->temp_raw  = (((int32_t)buf[3] << 12U) |
                      ((int32_t)buf[4] << 4U) |
                      ((int32_t)buf[5] >> 4U));

    raw->hum_raw   = ((int32_t)buf[6] << 8) | (int32_t)buf[7];

    return BME280_OK;
}


/**
 * @brief Compensate raw temperature.
 * @param[in]  dev    Device handle
 * @param[in]  adc_T  Raw temperature (20-bit)
 * @param[out] t_fine Fine temperature for P/H
 * @return Temperature in 0.01 °C
 */
int32_t bme280_compensate_temperature(bme280_dev_t *dev, int32_t adc_T, int32_t *t_fine)
{
    int32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
            ((int32_t)dev->calib.dig_T2)) >> 11;

    var2 = (((((adc_T >> 4) - ((int32_t)dev->calib.dig_T1)) *
              ((adc_T >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) *
            ((int32_t)dev->calib.dig_T3)) >> 14;

    *t_fine = var1 + var2;

    T = (*t_fine * 5 + 128) >> 8;
    return T; /* in 0.01 °C */
}


/**
 * @brief Compensate raw pressure.
 * @param[in] dev    Device handle
 * @param[in] adc_P  Raw pressure (20-bit)
 * @param[in] t_fine Fine temperature
 * @return Pressure in Pa (Q24.8 format)
 */
uint32_t bme280_compensate_pressure(bme280_dev_t *dev, int32_t adc_P, int32_t t_fine)
{
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dev->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dev->calib.dig_P3) >> 8) +
           ((var1 * (int64_t)dev->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1) *
           ((int64_t)dev->calib.dig_P1)) >> 33;

    if (var1 == 0) {
        return 0; /* avoid div by zero */
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);

    return (uint32_t)p; /* Pa (Q24.8 format) */
}

/**
 * @brief Compensate raw humidity.
 * @param[in] dev    Device handle
 * @param[in] adc_H  Raw humidity (16-bit)
 * @param[in] t_fine Fine temperature
 * @return Humidity in %RH (Q22.10 format)
 */
uint32_t bme280_compensate_humidity(bme280_dev_t *dev, int32_t adc_H, int32_t t_fine)
{
    int32_t v_x1_u32r;

    v_x1_u32r = t_fine - ((int32_t)76800);

    v_x1_u32r = (((((adc_H << 14) -
                    (((int32_t)dev->calib.dig_H4) << 20) -
                    (((int32_t)dev->calib.dig_H5) * v_x1_u32r)) + 16384) >> 15) *
                 (((((((v_x1_u32r * ((int32_t)dev->calib.dig_H6)) >> 10) *
                      (((v_x1_u32r * ((int32_t)dev->calib.dig_H3)) >> 11) + 32768)) >> 10) + 2097152) *
                   ((int32_t)dev->calib.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) *
                                  (v_x1_u32r >> 15)) >> 7) *
                                ((int32_t)dev->calib.dig_H1)) >> 4));

    if (v_x1_u32r < 0)
        v_x1_u32r = 0;
    if (v_x1_u32r > 419430400)
        v_x1_u32r = 419430400;

    return (uint32_t)(v_x1_u32r >> 12); /* %RH (Q22.10) */
}


/**
 * @brief Read and compensate all values (T, P, H).
 * @param[in]  dev   Device handle
 * @param[out] data  Compensated results
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_read_all(bme280_dev_t *dev, bme280_data_t *data)
{
    if (!dev || !data) {
        return BME280_E_PARAM;
    }

    bme280_raw_t raw;
    int32_t t_fine;
    int32_t t_comp;
    uint32_t p_comp, h_comp;

    if (bme280_read_raw(dev, &raw) != BME280_OK) {
        return BME280_E_COMM;
    }

    t_comp = bme280_compensate_temperature(dev, raw.temp_raw, &t_fine);
    data->temperature_c = t_comp / 100.0f;

    p_comp = bme280_compensate_pressure(dev, raw.press_raw, t_fine);
    data->pressure_pa = p_comp / 256.0f;

    h_comp = bme280_compensate_humidity(dev, raw.hum_raw, t_fine);
    data->humidity_rh = h_comp / 1024.0f;

    return BME280_OK;
}

/**
 * @brief Read only temperature.
 * @param[in]  dev   Device handle
 * @param[out] data  Output (temperature_c updated)
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_read_temperature(bme280_dev_t *dev, bme280_data_t *data)
{
    if (!dev || !data) {
        return BME280_E_PARAM;
    }

    bme280_raw_t raw;
    int32_t t_fine;
    int32_t t_comp;

    if (bme280_read_raw(dev, &raw) != BME280_OK) {
        return BME280_E_COMM;
    }

    t_comp = bme280_compensate_temperature(dev, raw.temp_raw, &t_fine);
    data->temperature_c = t_comp / 100.0f;

    return BME280_OK;
}

/**
 * @brief Read only pressure.
 * @param[in]  dev   Device handle
 * @param[out] data  Output (pressure_pa updated)
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_read_pressure(bme280_dev_t *dev, bme280_data_t *data)
{
    if (!dev || !data) {
        return BME280_E_PARAM;
    }

    bme280_raw_t raw;
    int32_t t_fine;
    int32_t t_comp;
    uint32_t p_comp;

    if (bme280_read_raw(dev, &raw) != BME280_OK) {
        return BME280_E_COMM;
    }

    t_comp = bme280_compensate_temperature(dev, raw.temp_raw, &t_fine);
    (void)t_comp;

    p_comp = bme280_compensate_pressure(dev, raw.press_raw, t_fine);
    data->pressure_pa = p_comp / 256.0f;

    return BME280_OK;
}

/**
 * @brief Read only humidity.
 * @param[in]  dev   Device handle
 * @param[out] data  Output (humidity_rh updated)
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_read_humidity(bme280_dev_t *dev, bme280_data_t *data)
{
    if (!dev || !data) {
        return BME280_E_PARAM;
    }

    bme280_raw_t raw;
    int32_t t_fine;
    int32_t t_comp;
    uint32_t h_comp;

    if (bme280_read_raw(dev, &raw) != BME280_OK) {
        return BME280_E_COMM;
    }

    t_comp = bme280_compensate_temperature(dev, raw.temp_raw, &t_fine);
    (void)t_comp;

    h_comp = bme280_compensate_humidity(dev, raw.hum_raw, t_fine);
    data->humidity_rh = h_comp / 1024.0f;

    return BME280_OK;
}

/**
 * @brief Read device ID (0x60 expected).
 * @param[in]  dev      Device handle
 * @param[out] chip_id  Returned ID
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_read_id(bme280_dev_t *dev, uint8_t *chip_id)
{
    if (!dev || !chip_id) {
        return BME280_E_PARAM;
    }

    if (dev->i2c_read(dev->dev_addr, BME280_REG_ID, chip_id, 1U) != 0) {
        return BME280_E_COMM;
    }

    return BME280_OK;
}

/**
 * @brief Issue a soft reset.
 * @param[in] dev  Device handle
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_soft_reset(bme280_dev_t *dev)
{
    if (!dev) {
        return BME280_E_PARAM;
    }

    uint8_t val = BME280_RESET_VALUE;
    if (dev->i2c_write(dev->dev_addr, BME280_REG_RESET, &val, 1U) != 0) {
        return BME280_E_COMM;
    }

    if (dev->delay_ms) {
        dev->delay_ms(2U);
    }

    return BME280_OK;
}

/**
 * @brief Put device into sleep mode.
 * @param[in] dev  Device handle
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_sleep(bme280_dev_t *dev)
{
    if (!dev) {
        return BME280_E_PARAM;
    }

    uint8_t val;
    if (dev->i2c_read(dev->dev_addr, BME280_REG_CTRL_MEAS, &val, 1U) != 0) {
        return BME280_E_COMM;
    }

    val &= ~0x03U; /* clear mode bits */
    if (dev->i2c_write(dev->dev_addr, BME280_REG_CTRL_MEAS, &val, 1U) != 0) {
        return BME280_E_COMM;
    }

    return BME280_OK;
}

/**
 * @brief Wake device into normal mode.
 * @param[in] dev  Device handle
 * @retval bme280_status_t Function result (BME280_OK on success, otherwise an error code)
 */
bme280_status_t bme280_wakeup(bme280_dev_t *dev)
{
    if (!dev) {
        return BME280_E_PARAM;
    }

    uint8_t val;
    if (dev->i2c_read(dev->dev_addr, BME280_REG_CTRL_MEAS, &val, 1U) != 0) {
        return BME280_E_COMM;
    }

    val = (val & ~0x03U) | BME280_MODE_NORMAL;
    if (dev->i2c_write(dev->dev_addr, BME280_REG_CTRL_MEAS, &val, 1U) != 0) {
        return BME280_E_COMM;
    }

    return BME280_OK;
}
