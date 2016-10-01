#include "MS5611.h"



MS5611::MS5611(I2C_HandleTypeDef *hi2c)
{
	this->hi2c = hi2c;
}

HAL_StatusTypeDef MS5611::Reset()
{
	uint8_t reset = 0x1E;
	HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(this->hi2c, this->ADDRESS, &reset, 1, HAL_MAX_DELAY);

	HAL_Delay(20);

	return status;
}

HAL_StatusTypeDef MS5611::Init()
{
	HAL_StatusTypeDef status;
	uint8_t data[2];

	for (uint8_t i = 0; i < 6; i++) {
		status = HAL_I2C_Mem_Read(this->hi2c, this->ADDRESS, 0xA2 + i * 2, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);

		if (status != HAL_OK)
			return status;

		C[i] = data[0] << 8;
		C[i] |= data[1];

		HAL_Delay(10);
	}

	return HAL_OK;
}

HAL_StatusTypeDef MS5611::GetData(int32_t * temperature, int32_t * pressure)
{
	HAL_StatusTypeDef status;
	uint32_t D1, D2;
	uint8_t conversionData[3];

	uint8_t convertD1 = 0x48;
	status = HAL_I2C_Master_Transmit(this->hi2c, this->ADDRESS, &convertD1, 1, HAL_MAX_DELAY);

	HAL_Delay(10);

	status = HAL_I2C_Mem_Read(this->hi2c, this->ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, conversionData, 3, HAL_MAX_DELAY);
	D1 = conversionData[0] << 16;
	D1 |= conversionData[1] << 8;
	D1 |= conversionData[2];

	uint8_t convertD2 = 0x58;
	status = HAL_I2C_Master_Transmit(this->hi2c, this->ADDRESS, &convertD2, 1, HAL_MAX_DELAY);

	HAL_Delay(10);

	status = HAL_I2C_Mem_Read(this->hi2c, this->ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, conversionData, 3, HAL_MAX_DELAY);
	D2 = conversionData[0] << 16;
	D2 |= conversionData[1] << 8;
	D2 |= conversionData[2];

	int32_t dT = D2 - (uint32_t)C[4] * pow(2, 8);
	int32_t TEMP = 2000 + dT * C[5] / pow(2, 23);
	int64_t OFF = (int64_t)C[1] * pow(2, 16) + (int64_t)(C[3] * dT) / pow(2, 7);
	int64_t SENS = (int64_t)C[0] * pow(2, 15) + (int64_t)(C[2] * dT) / pow(2, 8);

	int32_t TEMP2 = 0;
	int64_t OFF2 = 0;
	int64_t SENS2 = 0;

	if (TEMP < 2000) {
		TEMP2 = (dT * dT) / pow(2, 31);
		OFF2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 2;
		SENS2 = 5 * ((TEMP - 2000) * (TEMP - 2000)) / 4;
	}
	if (TEMP < -1500) {
		OFF2 = OFF2 + 7 * ((TEMP + 1500) * (TEMP + 1500));
		SENS2 = SENS2 + 11 * ((TEMP + 1500) * (TEMP + 1500)) / 2;
	}

	TEMP -= TEMP2;
	OFF -= OFF2;
	SENS -= SENS2;

	int32_t P = (((double)D1) * SENS / pow(2, 21) - OFF) / pow(2, 15);

	*temperature = TEMP;
	*pressure = P;

	return HAL_OK;
}


