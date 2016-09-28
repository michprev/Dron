#include <stm32f4xx_hal.h>
#include "MS5611.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

I2C_HandleTypeDef hi2c;

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

	hi2c.Instance = I2C1;
	hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c.Init.ClockSpeed = 100000;
	hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	hi2c.Init.OwnAddress1 = 0;
	hi2c.Init.OwnAddress2 = 0;

	HAL_I2C_Init(&hi2c);
}

int main(void)
{
	HAL_Init();
	I2cMaster_Init();

	MS5611 ms5611 = MS5611(&hi2c);
	ms5611.Init();

	int32_t press, temp;

	while (true) {
		ms5611.GetData(&temp, &press);

		printf("Temp: %d, press: %d\n", temp, press);

		HAL_Delay(100);
	}
}
