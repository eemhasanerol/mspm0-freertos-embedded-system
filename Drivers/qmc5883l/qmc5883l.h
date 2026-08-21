/*
 * qmc5883l.h
 * High-level API for QMC5883L 3-axis magnetometer.
 * See qmc5883l_defs.h for register map and option macros.
 *
 *  Created on: Sep 12, 2025
 *      Author: erol-tesla
 */

#ifndef QMC5883L_H_
#define QMC5883L_H_

#include <stdint.h>
#include <stddef.h>
#include "qmc5883l_defs.h"

/**
 * @brief Status codes for QMC5883L driver functions.
 */
typedef enum {
    QMC5883L_OK       =  0, /*!< Operation successful          */
    QMC5883L_E_COMM   = -1, /*!< Communication error (I2C)     */
    QMC5883L_E_PARAM  = -2, /*!< Invalid parameter             */
    QMC5883L_E_TIMEOUT= -3, /*!< Data not ready (timeout)      */
    QMC5883L_E_ID  = -4,     /*!< Invalid or unexpected chip ID */
    QMC5883L_E_ERROR = -5
} qmc5883l_status_t;

/* -------------------------------------------------------------------------- */
/* Raw data structure                                                         */
/* -------------------------------------------------------------------------- */
typedef struct {
    int16_t x;  /*!< Raw X axis */
    int16_t y;  /*!< Raw Y axis */
    int16_t z;  /*!< Raw Z axis */
} qmc5883l_raw_t;

/* -------------------------------------------------------------------------- */
/* Calibration parameters                                                     */
/* -------------------------------------------------------------------------- */
typedef struct {
    float offset_x;  /*!< Hard-iron offset */
    float offset_y;
    float offset_z;
    float scale_x;   /*!< Soft-iron scale factor */
    float scale_y;
    float scale_z;
} qmc5883l_calib_t;

/* -------------------------------------------------------------------------- */
/* Calibrated data (µT)                                                       */
/* -------------------------------------------------------------------------- */
typedef struct {
    float x_uT;
    float y_uT;
    float z_uT;
} qmc5883l_data_t;

/* -------------------------------------------------------------------------- */
/* Device handle                                                              */
/* -------------------------------------------------------------------------- */
typedef struct {
    uint8_t dev_addr;   /*!< 7-bit I2C address (default: 0x0D) */

    /* Cached CTRL1 fields (use macros from qmc5883l_defs.h) */
    uint8_t osr;        /*!< @ref QMC5883L_OSR_* */
    uint8_t rng;        /*!< @ref QMC5883L_RNG_* */
    uint8_t odr;        /*!< @ref QMC5883L_ODR_* */
    uint8_t mode;       /*!< @ref QMC5883L_MODE_* */

    /* Calibration */
    qmc5883l_calib_t calib;

    /* I2C hooks (blocking) */
    int32_t (*i2c_read)(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len);
    int32_t (*i2c_write)(uint8_t dev, uint8_t reg, const uint8_t *buf, uint16_t len);
    void    (*delay_ms)(uint32_t ms);
} qmc5883l_dev;
/* -------------------------------------------------------------------------- */
/* Public API                                                                 */
/* -------------------------------------------------------------------------- */

/**
 * @brief Initialize QMC5883L device.
 *
 * Verifies chip ID, issues soft reset, sets default set/reset period,
 * and writes CTRL1 using fields in @ref qmc5883l_dev.
 *
 * @param[in,out] dev Pointer to device handle
 * @return qmc5883l_status_t
 */
qmc5883l_status_t qmc5883l_init(qmc5883l_dev *dev);

/**
 * @brief Read raw magnetometer data.
 *
 * Reads 6 bytes starting at @ref QMC5883L_REG_X_LSB.
 *
 * @param[in]  dev Pointer to device handle
 * @param[out] raw Pointer to raw data struct
 * @return qmc5883l_status_t
 */
qmc5883l_status_t qmc5883l_read_raw(qmc5883l_dev *dev, qmc5883l_raw_t *raw);

/**
 * @brief Read calibrated field values in microTesla.
 *
 * Applies range scaling and calibration offsets to raw XYZ.
 *
 * @param[in]  dev  Pointer to device handle
 * @param[out] data Pointer to calibrated data struct (µT)
 * @return qmc5883l_status_t
 */
qmc5883l_status_t qmc5883l_read_data(qmc5883l_dev *dev, qmc5883l_data_t *data);

/**
 * @brief Compute compass heading in degrees [0, 360).
 *
 * Uses calibrated X/Y field values and applies local declination.
 *
 * @param[in]  dev             Pointer to device handle
 * @param[in]  declination_deg Declination in degrees (+E / −W)
 * @param[out] heading_deg     Output heading angle [0, 360)
 * @return qmc5883l_status_t
 */
qmc5883l_status_t qmc5883l_get_heading(qmc5883l_dev *dev, float *heading_deg);

#endif /* QMC5883L_INC_QMC5883L_H_ */