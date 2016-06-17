#ifndef __I2C_H
#define __I2C_H

#include <stm32f1xx_hal.h>

HAL_StatusTypeDef I2C_Write(I2C_HandleTypeDef *handle, uint16_t devAddress, uint16_t memAddress, uint8_t *data, uint16_t size);

#endif