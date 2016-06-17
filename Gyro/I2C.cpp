#include "I2C.h"

HAL_StatusTypeDef I2C_Write(I2C_HandleTypeDef *handle, uint16_t devAddress, uint16_t memAddress, uint8_t *data, uint16_t size) {
	return HAL_I2C_Mem_Write(handle, devAddress, memAddress, I2C_MEMADD_SIZE_8BIT, data, size, HAL_MAX_DELAY);
}