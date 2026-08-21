#include "platform.h"


SemaphoreHandle_t xI2CMutex = NULL;

/* Donanımsal I2C Fonksiyonları (İçlerine Mutex Koruması Ekledik) */
int32_t platform_i2c_read(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (xI2CMutex != NULL) {
        xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    }

    // 1) Register adresini yaz
    DL_I2C_flushControllerTXFIFO(I2C_INST);
    DL_I2C_fillControllerTXFIFO(I2C_INST, &reg, 1);

    DL_I2C_startControllerTransfer(I2C_INST, dev, DL_I2C_CONTROLLER_DIRECTION_TX, 1);

    uint32_t timeout = 200000;
    while ((DL_I2C_getControllerStatus(I2C_INST) != DL_I2C_CONTROLLER_STATUS_IDLE) && (--timeout));

    if (timeout == 0) {
        if (xI2CMutex != NULL) xSemaphoreGive(xI2CMutex);
        return -1;
    }

    // 2) Read başlat
    DL_I2C_startControllerTransfer(I2C_INST, dev, DL_I2C_CONTROLLER_DIRECTION_RX, len);

    for (int i = 0; i < len; i++) {
        uint32_t timeout = 100000;
        while (DL_I2C_isControllerRXFIFOEmpty(I2C_INST) && (--timeout));
        if (timeout == 0) { 
            if (xI2CMutex != NULL) xSemaphoreGive(xI2CMutex);
            return -1;
        }
        buf[i] = DL_I2C_receiveControllerData(I2C_INST);
    }

    while ((DL_I2C_getControllerStatus(I2C_INST) != DL_I2C_CONTROLLER_STATUS_IDLE) && (--timeout));

    if (xI2CMutex != NULL) {
        xSemaphoreGive(xI2CMutex);
    }

    if (timeout == 0) return -1;
    return 0;
}

int32_t platform_i2c_write(uint8_t dev, uint8_t reg, const uint8_t *buf, uint16_t len)
{
    uint8_t data[16];
    data[0] = reg;
    for (uint16_t i = 0; i < len; i++) {
        data[i + 1] = buf[i];
    }

    if (xI2CMutex != NULL) {
        xSemaphoreTake(xI2CMutex, portMAX_DELAY);
    }

    DL_I2C_flushControllerTXFIFO(I2C_INST);
    DL_I2C_fillControllerTXFIFO(I2C_INST, data, len + 1);

    DL_I2C_startControllerTransfer(I2C_INST, dev, DL_I2C_CONTROLLER_DIRECTION_TX, len + 1);

    uint32_t timeout = 100000;
    while ((DL_I2C_getControllerStatus(I2C_INST) != DL_I2C_CONTROLLER_STATUS_IDLE) && (--timeout));

    if (xI2CMutex != NULL) {
        xSemaphoreGive(xI2CMutex);
    }

    if (timeout == 0) return -1;
    return 0;
}

void platform_delay_ms(uint32_t ms)
{
    delay_cycles(ms * 32000);
}


