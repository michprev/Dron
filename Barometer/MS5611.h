#ifndef __MS5611_H
#define __MS5611_H

#include <stm32f4xx_hal.h>
#include <math.h>

class MS5611
{
public:
	MS5611(I2C_HandleTypeDef *hi2c);
	HAL_StatusTypeDef Reset();
	HAL_StatusTypeDef Init();
	HAL_StatusTypeDef GetData(int32_t *temperature, int32_t *pressure);
private:
	const uint8_t ADDRESS = 0xEE;
	uint16_t C[6];

	I2C_HandleTypeDef *hi2c;
};

#endif