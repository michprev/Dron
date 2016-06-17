#include "MPU6050.h"

void MPU6050::GPIO_Init() {
	if (__GPIOB_IS_CLK_DISABLED())
		__GPIOB_CLK_ENABLE();

	if (__I2C1_IS_CLK_DISABLED())
		__I2C1_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Speed = GPIO_SPEED_MEDIUM;
	gpio.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(GPIOB, &gpio);

	I2C_Handle.Instance = I2C1;
	I2C_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C_Handle.Init.ClockSpeed = 100000;
	I2C_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2C_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	I2C_Handle.Init.OwnAddress1 = 0;
	I2C_Handle.Init.OwnAddress2 = 0;

	HAL_I2C_Init(&I2C_Handle);
}

void MPU6050::IT_Init() {
	if (__GPIOA_IS_CLK_DISABLED())
		__GPIOA_CLK_ENABLE();

	GPIO_InitTypeDef exti;

	exti.Pin = GPIO_PIN_4;
	exti.Mode = GPIO_MODE_IT_RISING;
	exti.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &exti);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}

HAL_StatusTypeDef MPU6050::SetGyroRange(GyroRange range) {
	uint8_t buffer = range << 3;

	return I2C_Write(&I2C_Handle, MPU6050_ADDRESS, R_GYRO_CONFIG, &buffer, 1);
}

HAL_StatusTypeDef MPU6050::SetAccelRange(AccelRange range) {
	uint8_t buffer = range << 3;

	return I2C_Write(&I2C_Handle, MPU6050_ADDRESS, R_ACCEL_CONFIG, &buffer, 1);
}

HAL_StatusTypeDef MPU6050::Init() {
	GPIO_Init();
	IT_Init();

	uint8_t buffer;


	buffer = 0b10000000;
	// reset device
	if (I2C_Write(&I2C_Handle, MPU6050_ADDRESS, R_POWER_MANAGEMENT_1, &buffer, 1) != HAL_OK)
		return HAL_ERROR;

	HAL_Delay(100);

	buffer = 0b00000000;
	// wake up chip
	if (I2C_Write(&I2C_Handle, MPU6050_ADDRESS, R_POWER_MANAGEMENT_1, &buffer, 1) != HAL_OK)
		return HAL_ERROR;
}

extern "C" void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}