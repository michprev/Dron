#include "HMC5983.h"



HAL_StatusTypeDef HMC5983::I2C_Init()
{
	if (__GPIOB_IS_CLK_DISABLED())
		__GPIOB_CLK_ENABLE();

	if (__I2C1_IS_CLK_DISABLED())
		__I2C1_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	this->hi2c.Instance = I2C1;
	this->hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	this->hi2c.Init.ClockSpeed = 100000;
	this->hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	this->hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	this->hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	this->hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	this->hi2c.Init.OwnAddress1 = 0;
	this->hi2c.Init.OwnAddress2 = 0;

	return HAL_I2C_Init(&this->hi2c);
}

void HMC5983::Init()
{
	I2C_Init();
	uint8_t data;
	HAL_StatusTypeDef status;

	uint8_t addr = 0x00;

	do {
		status = HAL_I2C_Mem_Read(&this->hi2c, addr, 0x09, I2C_MEMADD_SIZE_8BIT, &data, 1, 1000);
		addr++;
	} while (status != 0 && addr <= 254);

	printf("Address %d\n", addr - 1);
}
