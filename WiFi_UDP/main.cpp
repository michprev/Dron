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

extern "C" void USART1_IRQHandler(void)
{
	uint8_t data = esp8266.huart.Instance->DR & (uint8_t)0x00FF;

	esp8266.WriteByte(&data);
}

extern "C" void HardFault_Handler(void)
{
	printf("hard fault\n");
}

void IPD_Callback(char *data) {
	//printf("IPD: %s\n", data);
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
	esp8266.output = true;
	esp8266.Init();
	printf("Init complete\n");

	uint8_t d[8] = { 20, 0, 0, 0, 0xDB, 0x03, 0x00, 0x00 };

	while (true) {
		HAL_IWDG_Refresh(&hiwdg);

		HAL_Delay(1000);
		esp8266.SendUDP(d, 8);

		esp8266.WaitReady(1);
	}
}
