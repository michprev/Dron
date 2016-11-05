#include <stm32f4xx_hal.h>
#include "ESP8266_UDP.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

const uint32_t UART_BUFFER_SIZE = 2048;
ESP8266_UDP esp8266 = ESP8266_UDP(UART_BUFFER_SIZE);
IWDG_HandleTypeDef hiwdg;

extern "C" void HardFault_Handler(void)
{
	printf("hard fault\n");
}

extern "C" void DMA2_Stream2_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&esp8266.hdma_usart1_rx);
}

uint32_t count = 0;

void IPD_Callback(uint8_t *data, uint16_t length) {
	count++;
	//printf("IPD: %s\n", data);
}

extern "C" void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	//printf("a\n");
}

extern "C" void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	//printf("a\n");
}

int main(void)
{
	HAL_Init();

	printf("init\n");

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hiwdg.Init.Reload = 2047;

	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		printf("Could not start watchdog\n");
	}

	// reset module
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	while (!esp8266.ready)
		esp8266.WaitReady();

	esp8266.IPD_Callback = &IPD_Callback;
	//esp8266.output = true;
	esp8266.Init();

	while (true) {
		HAL_IWDG_Refresh(&hiwdg);
		esp8266.WaitReady(0);
	}
}