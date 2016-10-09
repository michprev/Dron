#include <stm32f4xx_hal.h>

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

UART_HandleTypeDef huart;

extern "C" void UART4_IRQHandler(void)
{
	uint8_t data = huart.Instance->DR & (uint8_t)0x00FF;
}

int main(void)
{
	HAL_Init();

	if (__GPIOA_IS_CLK_DISABLED())
		__GPIOA_CLK_ENABLE();

	if (__UART4_IS_CLK_DISABLED())
		__UART4_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_0 | GPIO_PIN_1;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF8_UART4;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart.Instance = UART4;
	huart.Init.BaudRate = 9600;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&huart);

	/*__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);

	HAL_NVIC_SetPriority(UART4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(UART4_IRQn);*/

	uint8_t data[50] = { '\0' };

	

	while (true) {
		HAL_UART_Receive(&huart, data, 10, HAL_MAX_DELAY);
		printf("%s\n", data);
	}
}
