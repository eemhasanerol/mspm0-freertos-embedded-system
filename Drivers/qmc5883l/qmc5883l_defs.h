/*
 * qmc5883l_defs.h
 *
 * Definitions for QMC5883L magnetometer sensor.
 * Reference: QMC5883L Datasheet
 * Created on: Sep 12, 2025
 * Author: erol-tesla
 */

#ifndef QMC5883L_INC_QMC5883L_DEFS_H_
#define QMC5883L_INC_QMC5883L_DEFS_H_

#include <stdint.h>

/* -------------------------------------------------------------------------- */
/* Device information                                                         */
/* -------------------------------------------------------------------------- */
#define QMC5883L_I2C_ADDR            (0x0DU)  /*!< 7-bit I2C address */
#define QMC5883L_CHIP_ID_VALUE       (0xFFU)  /*!< Expected chip ID */

/* Example declination for Istanbul (~6° East) */
#define QMC5883L_DECLINATION_DEG     (6.0f)


/* -------------------------------------------------------------------------- */
/* Register map                                                               */
/* -------------------------------------------------------------------------- */
#define QMC5883L_REG_X_LSB           (0x00U)
#define QMC5883L_REG_X_MSB           (0x01U)
#define QMC5883L_REG_Y_LSB           (0x02U)
#define QMC5883L_REG_Y_MSB           (0x03U)
#define QMC5883L_REG_Z_LSB           (0x04U)
#define QMC5883L_REG_Z_MSB           (0x05U)
#define QMC5883L_REG_STATUS          (0x06U)
#define QMC5883L_REG_TOUT_LSB        (0x07U)  /*!< Optional: temperature out */
#define QMC5883L_REG_TOUT_MSB        (0x08U)
#define QMC5883L_REG_CTRL1           (0x09U)
#define QMC5883L_REG_CTRL2           (0x0AU)
#define QMC5883L_REG_SET_RESET       (0x0BU)
#define QMC5883L_REG_CHIP_ID         (0x0DU)


/* -------------------------------------------------------------------------- */
/* Status register bits (0x06)                                                */
/* -------------------------------------------------------------------------- */
/** @defgroup QMC5883L_STATUS Status bits
 *  @brief Status register (0x06) bit definitions
 *  @{
 */
#define QMC5883L_STATUS_DRDY         (1U << 0U)  /*!< Data ready */
#define QMC5883L_STATUS_OVL          (1U << 1U)  /*!< Overflow */
#define QMC5883L_STATUS_DOR          (1U << 2U)  /*!< Data overrun */
/** @} */


/* -------------------------------------------------------------------------- */
/* Control register 1 (0x09) fields                                           */
/* -------------------------------------------------------------------------- */
/** @defgroup QMC5883L_CTRL1 Control register 1 fields
 *  @brief OSR, range, ODR, mode settings
 *  @{
 */
/* Oversampling (OSR[7:6]) */
#define QMC5883L_OSR_512             (0x00U << 6U)
#define QMC5883L_OSR_256             (0x01U << 6U)
#define QMC5883L_OSR_128             (0x02U << 6U)
#define QMC5883L_OSR_064             (0x03U << 6U)

/* Range (RNG[5:4]) */
#define QMC5883L_RNG_2G              (0x00U << 4U)  /*!< ±2 Gauss */
#define QMC5883L_RNG_8G              (0x01U << 4U)  /*!< ±8 Gauss */

/* Output data rate (ODR[3:2]) */
#define QMC5883L_ODR_10HZ            (0x00U << 2U)
#define QMC5883L_ODR_50HZ            (0x01U << 2U)
#define QMC5883L_ODR_100HZ           (0x02U << 2U)
#define QMC5883L_ODR_200HZ           (0x03U << 2U)

/* Mode (MODE[1:0]) */
#define QMC5883L_MODE_STANDBY        (0x00U)
#define QMC5883L_MODE_CONTINUOUS     (0x01U)
/** @} */


/* -------------------------------------------------------------------------- */
/* Control register 2 (0x0A) bits                                             */
/* -------------------------------------------------------------------------- */
/** @defgroup QMC5883L_CTRL2 Control register 2 bits
 *  @brief Reset, rollover, and interrupt enable
 *  @{
 */
#define QMC5883L_SOFT_RST            (1U << 7U)  /*!< Soft reset */
#define QMC5883L_ROL_PNT             (1U << 6U)  /*!< Pointer rollover */
#define QMC5883L_INT_ENB             (1U << 0U)  /*!< DRDY pin enable */
/** @} */


/* -------------------------------------------------------------------------- */
/* Set/Reset period                                                           */
/* -------------------------------------------------------------------------- */
/** @defgroup QMC5883L_SETRESET Set/reset period
 *  @brief Recommended set/reset value
 *  @{
 */
#define QMC5883L_SET_RESET_DEFAULT   (0x01U)
/** @} */

#endif /* QMC5883L_INC_QMC5883L_DEFS_H_ */