#ifndef __HMC5983_H
#define __HMC5983_H

#include <stm32f4xx_hal.h>

class HMC5983
{
private:
	const uint8_t ADDRESS = 0x1E;
	I2C_HandleTypeDef hi2c;

	HAL_StatusTypeDef I2C_Init();
public:
	void Init();

};

#endif