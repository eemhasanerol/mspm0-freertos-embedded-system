/*
 * qmc5883l.c
 *
 *  Created on: Sep 12, 2025
 *      Author: erol-tesla
 */

#include <math.h>
#include "qmc5883l.h"

qmc5883l_status_t qmc5883l_init(qmc5883l_dev *dev)
{
    if ((dev == NULL) || (dev->i2c_read == NULL) || (dev->i2c_write == NULL) || (dev->delay_ms == NULL))
    {
        return QMC5883L_E_PARAM;
    }

    uint8_t val = 0;

    if(dev->i2c_read(dev->dev_addr, QMC5883L_REG_CHIP_ID, &val, 1U) != 0)
    {
        return QMC5883L_E_COMM;
    }

    if(val != QMC5883L_CHIP_ID_VALUE)
    {
        return QMC5883L_E_ID;
    }


    /* Soft reset */
    val = QMC5883L_SOFT_RST;
    if (dev->i2c_write(dev->dev_addr, QMC5883L_REG_CTRL2, &val, 1U) != 0)
    {
        return QMC5883L_E_COMM;
    }

    dev->delay_ms(10U);

    /* Set/Reset period */
    val = QMC5883L_SET_RESET_DEFAULT;
    if (dev->i2c_write(dev->dev_addr, QMC5883L_REG_SET_RESET, &val, 1U) != 0)
    {
        return QMC5883L_E_COMM;
    }
    
    /* Configure CTRL1 */
    val = (uint8_t)(dev->mode | dev->odr | dev->osr | dev->rng);
    if (dev->i2c_write(dev->dev_addr, QMC5883L_REG_CTRL1, &val, 1U) != 0)
    {
        return QMC5883L_E_COMM;
    }

    /* smdilik kaldırdım. 
    
        val = QMC5883L_ROL_PNT;
    if (dev->i2c_write(dev->dev_addr, QMC5883L_REG_CTRL2, &val, 1U) != 0)
    {
        return QMC5883L_E_COMM;
    }Release reset */

    return QMC5883L_OK;
}


static qmc5883l_status_t qmc5883l_get_status(qmc5883l_dev *dev, uint8_t *status)
{
    if ((dev == NULL) || (status == NULL))
    {
        return QMC5883L_E_PARAM;
    }

    if (dev->i2c_read(dev->dev_addr, QMC5883L_REG_STATUS, status, 1U) != 0)
    {
        return QMC5883L_E_COMM;
    }

    return QMC5883L_OK;
}


qmc5883l_status_t qmc5883l_read_raw(qmc5883l_dev *dev, qmc5883l_raw_t *raw)
{
    if ((dev == NULL) || (raw == NULL))
    {
        return QMC5883L_E_PARAM;
    }

    uint8_t buf[6], status = 0;

    if (qmc5883l_get_status(dev, &status) != QMC5883L_OK)
    {
        return QMC5883L_E_COMM;
    }

    /* DRDY = 1 -> data ready. Eğer 0 ise henüz hazır değil */
    if ((status & QMC5883L_STATUS_DRDY) == 0)
    {
        return QMC5883L_E_TIMEOUT;
    }

    /* Overflow / data overrun */
    if (status & QMC5883L_STATUS_OVL)
    {
        return QMC5883L_E_ERROR;
    }

    if (dev->i2c_read(dev->dev_addr, QMC5883L_REG_X_LSB, buf, 6U) != 0)
    {
        return QMC5883L_E_COMM;
    }

    raw->x = (int16_t)((buf[1] << 8U) | buf[0]);
    raw->y = (int16_t)((buf[3] << 8U) | buf[2]);
    raw->z = (int16_t)((buf[5] << 8U) | buf[4]);

    return QMC5883L_OK;
}


qmc5883l_status_t qmc5883l_read_data(qmc5883l_dev *dev, qmc5883l_data_t *data)
{
    if ((dev == NULL) || (data == NULL))
    {
        return QMC5883L_E_PARAM;
    }

    qmc5883l_raw_t raw;
    qmc5883l_status_t status = qmc5883l_read_raw(dev, &raw);
    if (status != QMC5883L_OK)
    {
        return status;
    }

    /* Select sensitivity */
    float lsb_per_gauss = (dev->rng == QMC5883L_RNG_2G) ? 12000.0f : 3000.0f;
    float scale = 100.0f / lsb_per_gauss;  /* 1 Gauss = 100 µT */

    /* Apply hard-iron offset */
    raw.x -= (int16_t)dev->calib.offset_x;
    raw.y -= (int16_t)dev->calib.offset_y;
    raw.z -= (int16_t)dev->calib.offset_z;

    /* Raw to µT */
    float x = ((float)raw.x) * scale;
    float y = ((float)raw.y) * scale;
    float z = ((float)raw.z) * scale;

    /* Apply soft-iron scale */
    x *= dev->calib.scale_x;
    y *= dev->calib.scale_y;
    z *= dev->calib.scale_z;

    data->x_uT = x;
    data->y_uT = y;
    data->z_uT = z;

    return QMC5883L_OK;
}


qmc5883l_status_t qmc5883l_get_heading(qmc5883l_dev *dev, float *heading_deg)
{
    if ((dev == NULL) || (heading_deg == NULL))
    {
        return QMC5883L_E_PARAM;
    }

    qmc5883l_data_t v;
    qmc5883l_status_t st = qmc5883l_read_data(dev, &v);
    if (st != QMC5883L_OK)
    {
        return st;
    }

    float heading_rad = atan2f(v.y_uT, v.x_uT);
    
    const float deg2rad = (float)M_PI / 180.0f;
    heading_rad += QMC5883L_DECLINATION_DEG * deg2rad;

    const float two_pi = 2.0f * (float)M_PI;
    while (heading_rad < 0.0f)     heading_rad += two_pi;
    while (heading_rad >= two_pi)  heading_rad -= two_pi;

    *heading_deg = heading_rad * (180.0f / (float)M_PI);


    return QMC5883L_OK;
}