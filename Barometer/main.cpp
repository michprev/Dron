#include <stm32f4xx_hal.h>
#include <cmath>

#ifdef __cplusplus
extern "C"
#endif
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

I2C_HandleTypeDef i2c;

void I2cMaster_Init(void)
{
	if (__GPIOB_IS_CLK_DISABLED())
		__GPIOB_CLK_ENABLE();

	if (__I2C1_IS_CLK_DISABLED())
		__I2C1_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Speed = GPIO_SPEED_MEDIUM;
	gpio.Pull = GPIO_PULLUP;
	gpio.Alternate = GPIO_AF4_I2C1;

	HAL_GPIO_Init(GPIOB, &gpio);

	i2c.Instance = I2C1;
	i2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	i2c.Init.ClockSpeed = 100000;
	i2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	i2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	i2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	i2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	i2c.Init.OwnAddress1 = 0;
	i2c.Init.OwnAddress2 = 0;

	HAL_I2C_Init(&i2c);
}

int main(void)
{
	HAL_Init();

	I2cMaster_Init();

	uint8_t reset = 0x1E;
	if (HAL_I2C_Master_Transmit(&i2c, 0x77 << 1, &reset, 1, HAL_MAX_DELAY) != HAL_OK)
		printf("Reset error\n");

	HAL_Delay(20);

	uint16_t C1, C2, C3, C4, C5, C6;
	uint8_t data[2];

	HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0xA2, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	C1 = data[0] << 8;
	C1 |= data[1];
	printf("C1: %d\n", C1);
	
	HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0xA4, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	C2 = data[0] << 8;
	C2 |= data[1];
	printf("C2: %d\n", C2);

	HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0xA6, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	C3 = data[0] << 8;
	C3 |= data[1];
	printf("C3: %d\n", C3);

	HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0xA8, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	C4 = data[0] << 8;
	C4 |= data[1];
	printf("C4: %d\n", C4);

	HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0xAA, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	C5 = data[0] << 8;
	C5 |= data[1];
	printf("C5: %d\n", C5);

	HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0xAC, I2C_MEMADD_SIZE_8BIT, data, 2, HAL_MAX_DELAY);
	C6 = data[0] << 8;
	C6 |= data[1];
	printf("C6: %d\n", C6);

	while (true) {
		uint32_t D1, D2;
		uint8_t conversionData[4];

		uint8_t convertD1 = 0x48;
		if (HAL_I2C_Master_Transmit(&i2c, 0x77 << 1, &convertD1, 1, HAL_MAX_DELAY) != HAL_OK)
			printf("Convert D1 error\n");

		HAL_Delay(10);

		HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0x00, I2C_MEMADD_SIZE_8BIT, conversionData, 4, HAL_MAX_DELAY);
		D1 = conversionData[0] << 24;
		D1 |= conversionData[1] << 16;
		D1 |= conversionData[2] << 8;
		D1 |= conversionData[3];

		//printf("D1: %u\n", D1);


		uint8_t convertD2 = 0x58;
		if (HAL_I2C_Master_Transmit(&i2c, 0x77 << 1, &convertD2, 1, HAL_MAX_DELAY) != HAL_OK)
			printf("Convert D1 error\n");

		HAL_Delay(10);

		HAL_I2C_Mem_Read(&i2c, 0x77 << 1, 0x00, I2C_MEMADD_SIZE_8BIT, conversionData, 4, HAL_MAX_DELAY);
		D2 = conversionData[0] << 24;
		D2 |= conversionData[1] << 16;
		D2 |= conversionData[2] << 8;
		D2 |= conversionData[3];

		//printf("D2: %u\n", D2);


		int32_t dT = D2 - C5 * pow(2, 8);
		int32_t TEMP = 2000 + dT * C6 / pow(2, 23);
		int64_t OFF = C2 * pow(2, 16) + ((double)(C4 * dT)) / pow(2, 7);
		int64_t SENS = C1 * pow(2, 15) + ((double)(C3 * dT)) / pow(2, 8);
		int32_t P = (((double)D1) * SENS / pow(2, 21) - OFF) / pow(2, 15);

		printf("P: %d, T: %d\n", P, TEMP);


		HAL_Delay(100);
	}
}
