#include <stm32f4xx_hal.h>
#include "Buffer.h"
#include <string.h>

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

const uint32_t UART_BUFFER_SIZE = 2048;

Buffer UART_Buffer = Buffer(UART_BUFFER_SIZE);
UART_HandleTypeDef huart;

extern "C" void USART1_IRQHandler(void)
{
	uint8_t data = huart.Instance->DR & (uint8_t)0x00FF;

	UART_Buffer.WriteByte(&data);
}

void SMS_ReadyCallback() {
	//printf("D: SMS ready\n");
}

void Call_ReadyCallback() {
	//printf("D: Call ready\n");
}

int main(void)
{
	HAL_Init();

	__GPIOA_CLK_ENABLE();
	__USART1_CLK_ENABLE();

	GPIO_InitTypeDef rst;
	rst.Pin = GPIO_PIN_5;
	rst.Mode = GPIO_MODE_OUTPUT_PP;
	rst.Pull = GPIO_PULLUP;;
	rst.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &rst);


	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	huart.Instance = USART1;
	huart.Init.BaudRate = 9600;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&huart);

	__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);

	UART_Buffer.SMS_Callback = &SMS_ReadyCallback;
	UART_Buffer.Call_Callback = &Call_ReadyCallback;

	// reset module
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

	/*while (!UART_Buffer.ready)
		UART_Buffer.WaitReady();*/

	UART_Buffer.Init(&huart);

	while (!UART_Buffer.smsReady)
		UART_Buffer.WaitReady();

	UART_Buffer.TestSMS(&huart);
	printf("Init complete\n");
	UART_Buffer.Print();
	while (true) {
		UART_Buffer.WaitReady();
		HAL_Delay(1000);
	}
}
