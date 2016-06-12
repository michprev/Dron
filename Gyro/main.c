#include <stm32f1xx_hal.h>
#include <stdbool.h>

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void EXTI4_IRQHandler(void)
{
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}

void GPIO_Init();
void UART1_Init();
void I2C1_Init();
void EXTI_Init();

UART_HandleTypeDef UART1_Handle;
I2C_HandleTypeDef I2C1_Handle;


int main(void)
{
	HAL_Init();

	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__USART1_CLK_ENABLE();
	__I2C1_CLK_ENABLE();
	
	GPIO_Init();
	UART1_Init();
	I2C1_Init();
	EXTI_Init();

	HAL_Delay(500);

	uint8_t buffer = 0;
	uint8_t status = 0;

	status += HAL_I2C_Mem_Write(&I2C1_Handle, 0xD0, 0x6B, I2C_MEMADD_SIZE_8BIT, &buffer, 1, HAL_MAX_DELAY);
	status += HAL_I2C_Mem_Write(&I2C1_Handle, 0xD0, 0x1B, I2C_MEMADD_SIZE_8BIT, &buffer, 1, HAL_MAX_DELAY);
	status += HAL_I2C_Mem_Write(&I2C1_Handle, 0xD0, 0x1C, I2C_MEMADD_SIZE_8BIT, &buffer, 1, HAL_MAX_DELAY);

	buffer = 1;

	status += HAL_I2C_Mem_Write(&I2C1_Handle, 0xD0, 0x38, I2C_MEMADD_SIZE_8BIT, &buffer, 1, HAL_MAX_DELAY);

	while (true)
	{
		HAL_I2C_Mem_Read(&I2C1_Handle, 0xD0, 0x3A, I2C_MEMADD_SIZE_8BIT, &buffer, 1, HAL_MAX_DELAY);
		HAL_UART_Transmit(&UART1_Handle, &buffer, 1, HAL_MAX_DELAY);

		if (status == 0)
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
		HAL_Delay(500);
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	uint8_t buffer = 55;
	HAL_UART_Transmit(&UART1_Handle, &buffer, 1, HAL_MAX_DELAY);
}

void GPIO_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void UART1_Init(void) {
	GPIO_InitTypeDef tx;
	tx.Pin = GPIO_PIN_9;
	tx.Mode = GPIO_MODE_AF_PP;
	tx.Speed = GPIO_SPEED_HIGH;
	tx.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &tx);


	GPIO_InitTypeDef rx;
	rx.Pin = GPIO_PIN_10;
	rx.Mode = GPIO_MODE_AF_INPUT;
	rx.Speed = GPIO_SPEED_HIGH;
	rx.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &rx);


	UART1_Handle.Instance = USART1;
	UART1_Handle.Init.BaudRate = 9600;
	UART1_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	UART1_Handle.Init.StopBits = UART_STOPBITS_1;
	UART1_Handle.Init.Parity = UART_PARITY_NONE;
	UART1_Handle.Init.Mode = UART_MODE_TX_RX;
	UART1_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART1_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART1_Handle);
}

void I2C1_Init() {
	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Speed = GPIO_SPEED_MEDIUM;
	gpio.Pull = GPIO_PULLUP;

	HAL_GPIO_Init(GPIOB, &gpio);

	
	I2C1_Handle.Instance = I2C1;
	I2C1_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	I2C1_Handle.Init.ClockSpeed = 100000;
	I2C1_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	I2C1_Handle.Init.DutyCycle = I2C_DUTYCYCLE_2;
	I2C1_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	I2C1_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	I2C1_Handle.Init.OwnAddress1 = 0;
	I2C1_Handle.Init.OwnAddress2 = 0;

	HAL_I2C_Init(&I2C1_Handle);
}

void EXTI_Init() {
	GPIO_InitTypeDef exti;

	exti.Pin = GPIO_PIN_4;
	exti.Mode = GPIO_MODE_IT_RISING;
	exti.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOA, &exti);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);
}
