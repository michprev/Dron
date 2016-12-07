#include <stm32f4xx_hal.h>
#include "NEO_M8N.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

NEO_M8N neo = NEO_M8N(2048);

extern "C" void DMA2_Stream2_IRQHandler(void)
{
	//HAL_DMA_IRQHandler(&neo.hdma_usart1_rx);
}

extern "C" void HardFault_Handler(void)
{
	printf("hard fault\n");
}

extern "C" void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	neo.writePos = 1024;
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	neo.writePos = 0;
}

int main(void)
{
	HAL_Init();

	neo.Init();

	//HAL_UART_Transmit(&neo.huart, tmp, 28, HAL_MAX_DELAY);

	HAL_UART_Receive_DMA(&neo.huart, neo.data, 2048);

	while (true) {
		HAL_Delay(1000);
		//neo.ParseData();
		//printf("%c\n", neo.data[0]);
	}
}
