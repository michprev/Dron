#include <stm32f1xx_hal.h>
#include <stdbool.h>
#include "stringBuffer.h"

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

void Reset_Init();
void GPIO_Init();
void UART1_Init();
void UART2_Init();
void DMA_UART1_Init();
void DMA_UART2_Init();

UART_HandleTypeDef UART1_Handle;
UART_HandleTypeDef UART2_Handle;
DMA_HandleTypeDef DMA_UART1_Rx;
DMA_HandleTypeDef DMA_UART2_Rx;


char UART1_RxBuffer;
char UART2_RxBuffer;

#define BUFFER_SIZE 1024

volatile char UART1_Buffer[BUFFER_SIZE];
volatile char UART2_Buffer[BUFFER_SIZE];
volatile uint8_t UART1_Length;
volatile uint8_t UART2_Length;

/* PC13, PC14, PC15 limited output!!*/

int main(void)
{
	HAL_Init();

	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__USART1_CLK_ENABLE();
	__USART2_CLK_ENABLE();
	__DMA1_CLK_ENABLE();

	//Reset_Init();
	GPIO_Init();
	UART1_Init();
	UART2_Init();
	DMA_UART1_Init();
	DMA_UART2_Init();

	UART1_Length = 0;
	UART2_Length = 0;

	HAL_UART_Receive_DMA(&UART1_Handle, &UART1_RxBuffer, 1);
	HAL_UART_Receive_DMA(&UART2_Handle, &UART2_RxBuffer, 1);

	int c = 0;

	char Tx1[BUFFER_SIZE];
	char Tx2[BUFFER_SIZE];
	uint8_t Tx1_Pos = 0;
	uint8_t Tx2_Pos = 0;
	uint8_t tmp1, tmp2;

	while (true)
	{
		c++;
		tmp1 = UART1_Length;
		tmp2 = UART2_Length;
		if (tmp1 > Tx1_Pos) {
			memcpy(Tx1, UART1_Buffer + Tx1_Pos, tmp1 - Tx1_Pos);
			HAL_UART_Transmit(&UART2_Handle, Tx1, tmp1 - Tx1_Pos, HAL_MAX_DELAY);
			Tx1_Pos = tmp1;
		}
		else if (tmp1 != Tx1_Pos) {
			memcpy(Tx1, UART1_Buffer + Tx1_Pos, BUFFER_SIZE - Tx1_Pos);
			memcpy(Tx1 + BUFFER_SIZE - Tx1_Pos, UART1_Buffer, tmp1);
			HAL_UART_Transmit(&UART2_Handle, Tx1, BUFFER_SIZE - Tx1_Pos + tmp1, HAL_MAX_DELAY);
			Tx1_Pos = tmp1;
		}

		if (tmp2 > Tx2_Pos) {
			memcpy(Tx2, UART2_Buffer + Tx2_Pos, tmp2 - Tx2_Pos);
			HAL_UART_Transmit(&UART1_Handle, Tx2, tmp2 - Tx2_Pos, HAL_MAX_DELAY);
			Tx2_Pos = tmp2;
		}
		else if (tmp2 != Tx2_Pos) {
			memcpy(Tx2, UART2_Buffer + Tx2_Pos, BUFFER_SIZE - Tx2_Pos);
			memcpy(Tx2 + BUFFER_SIZE - Tx2_Pos, UART2_Buffer, tmp2);
			HAL_UART_Transmit(&UART1_Handle, Tx2, BUFFER_SIZE - Tx2_Pos + tmp2, HAL_MAX_DELAY);
			Tx2_Pos = tmp2;
		}

		if (c == 50000) {
			HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_8);
			c = 0;
		}
	}
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART1) {
		if (UART1_RxBuffer != '\0') {
			UART1_Buffer[UART1_Length] = UART1_RxBuffer;
			UART1_Length++;
			UART1_Length %= BUFFER_SIZE;
		}
	}
	else if (huart->Instance == USART2) {
		if (UART2_RxBuffer != '\0' && UART2_RxBuffer != '\r') {
			UART2_Buffer[UART2_Length] = UART2_RxBuffer;
			UART2_Length++;
			UART2_Length %= BUFFER_SIZE;
		}
	}
}

void Reset_Init()
{
	GPIO_InitTypeDef GPIO_InitStructure;
	/*GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);*/
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

/* NOT 5V TOLERANT !! */
void UART2_Init() {
	GPIO_InitTypeDef tx;
	tx.Pin = GPIO_PIN_2;
	tx.Mode = GPIO_MODE_AF_PP;
	tx.Speed = GPIO_SPEED_HIGH;
	tx.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &tx);

	GPIO_InitTypeDef rx;
	rx.Pin = GPIO_PIN_3;
	rx.Mode = GPIO_MODE_AF_INPUT;
	rx.Speed = GPIO_SPEED_HIGH;
	rx.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOA, &rx);

	UART2_Handle.Instance = USART2;
	UART2_Handle.Init.BaudRate = 9600;
	UART2_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	UART2_Handle.Init.StopBits = UART_STOPBITS_1;
	UART2_Handle.Init.Parity = UART_PARITY_NONE;
	UART2_Handle.Init.Mode = UART_MODE_TX_RX;
	UART2_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART2_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART2_Handle);
}

void DMA_UART1_Init() {
	DMA_UART1_Rx.Instance = DMA1_Channel5;
	DMA_UART1_Rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_UART1_Rx.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_UART1_Rx.Init.MemInc = DMA_MINC_ENABLE;
	DMA_UART1_Rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DMA_UART1_Rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA_UART1_Rx.Init.Mode = DMA_CIRCULAR;
	DMA_UART1_Rx.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&DMA_UART1_Rx);

	__HAL_LINKDMA(&UART1_Handle, hdmarx, DMA_UART1_Rx);

	HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);
}

void DMA_UART2_Init() {
	DMA_UART2_Rx.Instance = DMA1_Channel6;
	DMA_UART2_Rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA_UART2_Rx.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA_UART2_Rx.Init.MemInc = DMA_MINC_ENABLE;
	DMA_UART2_Rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DMA_UART2_Rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA_UART2_Rx.Init.Mode = DMA_CIRCULAR;
	DMA_UART2_Rx.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&DMA_UART2_Rx);

	__HAL_LINKDMA(&UART2_Handle, hdmarx, DMA_UART2_Rx);

	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}

void DMA1_Channel5_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA_UART1_Rx);
}

void DMA1_Channel6_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA_UART2_Rx);
}