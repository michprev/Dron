#include <stm32f4xx_hal.h>
#include "NEO_M8N.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

NEO_M8N neo = NEO_M8N(2048);

extern "C" void DMA1_Stream1_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&neo.hdma_usart3_rx);
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

	uint8_t tmp[] = { 0xB5, 0x62, 0x06, 0x00, 0x14, 0x00, 0x01, 0x00, 0x00, 0x00, 0xD0, 0x08, 0x00, 0x00/**/, 0x00, 0xC2, 0x01, 0x00,/**/ 0x07, 0x00, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0xC9 };
	// 28

	//HAL_UART_Transmit(&neo.huart, tmp, 28, HAL_MAX_DELAY);

	HAL_UART_Receive_DMA(&neo.huart, neo.data, 2048);

	while (true) {
		neo.ParseData();
		//printf("%c\n", neo.data[0]);
	}
}
