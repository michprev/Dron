#include <stm32f1xx_hal.h>
#include <stdbool.h>
#include "MPU6050.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void GPIO_Init();
void UART1_Init();

UART_HandleTypeDef UART1_Handle;

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";

int main(void)
{
	HAL_Init();
	cMPU6050 mpu;
	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__USART1_CLK_ENABLE();
	
	GPIO_Init();
	UART1_Init();

	mpu.Init();

	while (true)
	{
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
		HAL_Delay(500);
	}
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
