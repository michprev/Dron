#include <stm32f1xx_hal.h>
#include <string.h>
#include "stringBuffer.h"

#ifdef __cplusplus
extern "C"
#endif
	

void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

#define Log(x) HAL_UART_Transmit(&UART1_Handle, (uint8_t*) x, strlen(x), HAL_MAX_DELAY)
#define LogDMA(x)  HAL_UART_Transmit_DMA(&UART1_Handle, (uint8_t*) x, strlen(x))
//#define DebugWrite(x) HAL_UART_Transmit(&UART2_Handle, (uint8_t*) x, strlen(x), HAL_MAX_DELAY)
	
DMA_HandleTypeDef DMA2_Handle;
DMA_HandleTypeDef DMA1_Handle;

UART_HandleTypeDef UART1_Handle;
UART_HandleTypeDef UART2_Handle;

void Write(char *str) {
	do {
		HAL_UART_Transmit(&UART2_Handle, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
	} while (waitForRxComplete() != 0);	
}

void DebugWrite(char *str) {
	HAL_UART_Transmit(&UART2_Handle, (uint8_t*)str, strlen(str), HAL_MAX_DELAY);
}
	
#ifdef __cplusplus
extern "C"
#endif

void DMA1_Channel4_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA1_Handle);
}

#ifdef __cplusplus
extern "C"
#endif

void DMA1_Channel6_IRQHandler(void)
{
	HAL_DMA_IRQHandler(&DMA2_Handle);
}

void GPIO_Init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_7;
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
	UART1_Handle.Init.BaudRate = 11520;
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
	UART2_Handle.Init.BaudRate = 115200;
	UART2_Handle.Init.WordLength = UART_WORDLENGTH_8B;
	UART2_Handle.Init.StopBits = UART_STOPBITS_1;
	UART2_Handle.Init.Parity = UART_PARITY_NONE;
	UART2_Handle.Init.Mode = UART_MODE_TX_RX;
	UART2_Handle.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	UART2_Handle.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&UART2_Handle);
}

void DMA1_Init() {
	HAL_NVIC_DisableIRQ(DMA1_Channel4_IRQn);

	DMA1_Handle.Instance = DMA1_Channel4;
	DMA1_Handle.Init.Direction = DMA_MEMORY_TO_PERIPH;
	DMA1_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA1_Handle.Init.MemInc = DMA_MINC_ENABLE;
	DMA1_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DMA1_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA1_Handle.Init.Mode = DMA_CIRCULAR;
	DMA1_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&DMA1_Handle);

	__HAL_LINKDMA(&UART1_Handle, hdmatx, DMA1_Handle);

	HAL_NVIC_SetPriority(DMA1_Channel4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel4_IRQn);
}

void DMA2_Init() {
	HAL_NVIC_DisableIRQ(DMA1_Channel6_IRQn);

	DMA2_Handle.Instance = DMA1_Channel6;
	DMA2_Handle.Init.Direction = DMA_PERIPH_TO_MEMORY;
	DMA2_Handle.Init.PeriphInc = DMA_PINC_DISABLE;
	DMA2_Handle.Init.MemInc = DMA_MINC_ENABLE;
	DMA2_Handle.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	DMA2_Handle.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	DMA2_Handle.Init.Mode = DMA_CIRCULAR;
	DMA2_Handle.Init.Priority = DMA_PRIORITY_MEDIUM;
	HAL_DMA_Init(&DMA2_Handle);

	__HAL_LINKDMA(&UART2_Handle, hdmarx, DMA2_Handle);

	HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
}


stringBuffer buff;

char rxBuffer;
char txBuffer = '\0';

char rxBuffer2[10];

#define BUFF_SIZE 500

volatile char buffer[BUFF_SIZE] = { '\0' };
int rxPos = 0;
int txPos = 0;


int main(void)
{
	HAL_Init();

	__GPIOA_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
	__USART1_CLK_ENABLE();
	__USART2_CLK_ENABLE();
	__DMA1_CLK_ENABLE();

	GPIO_Init();
	UART1_Init();
	UART2_Init();
	DMA1_Init();
	DMA2_Init();
	
	stringBufferInit(&buff);

	HAL_UART_Receive_DMA(&UART2_Handle, (uint8_t*)rxBuffer2, 10);

	//HAL_UART_Transmit_DMA(&UART1_Handle, (uint8_t*)&txBuffer, 1);

	DebugWrite("AT+GMR\r\n");

	/*HAL_Delay(5000);

	HAL_UART_Transmit(&UART1_Handle, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);*/

	/*Write("AT+GMR\r\n");
	
	Write("AT+CWMODE_CUR=1\r\n");

	//Write("AT+CWLAP\r\n");

	Write("AT+CWJAP_CUR=\"FreeGymKt\",\"gkt2010a\"\r\n");
	

	Write("AT+CIPSTART=\"TCP\",\"google.com\",80\r\n");

	char message[] = "GET HTTP/1.1\r\nHost: google.com\r\n";
	char length[10] = { '\0' };
	char header[] = "AT+CIPSEND=";
	itoa(strlen(message), length, 10);

	strcat(header, length);
	strcat(header, "\r\n");

	HAL_UART_Transmit(&UART2_Handle, (uint8_t*)header, strlen(header), HAL_MAX_DELAY);

	while (tmp[pos][0] != '>');

	pos = 0;
	rxBuffPointer = tmp[0];

	Write(message);*/

	while (true)
	{
		if (buff.count > 0) {
			HAL_UART_Transmit(&UART1_Handle, buff.first->str, 10, HAL_MAX_DELAY);
			stringBufferRemove(&buff);
		}
		HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7);
		HAL_Delay(500);
	}
}

#ifdef __cplusplus
extern "C"
#endif

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == USART2) {
		stringBufferAdd(&buff);
		strncpy(buff.last->str, rxBuffer2, 10);
	}
	
}

#ifdef __cplusplus
extern "C"
#endif

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	//Log("error\n");
}

/*uint8_t waitForRxComplete() {
	uint8_t status;

	while (true) {
		if (pos > 0) {
			size_t length = strlen(tmp[pos - 1]);
			if (length >= 4 && tmp[pos - 1][length - 4] == 'O' && tmp[pos - 1][length - 3] == 'K') {
				status = 0;
				break;
			}
			else if (length >= 6 && tmp[pos - 1][length - 6] == 'F' && tmp[pos - 1][length - 5] == 'A' && tmp[pos - 1][length - 4] == 'I' && tmp[pos - 1][length - 3] == 'L') {
				status = -1;
				break;
			}
			else if (length >= 7 && tmp[pos - 1][length - 7] == 'E' && tmp[pos - 1][length - 6] == 'R' && tmp[pos - 1][length - 5] == 'R' && tmp[pos - 1][length - 4] == 'O' && tmp[pos - 1][length - 3] == 'R') {
				status = -1;
				break;
			}
		}
	}

	for (int i = 0; i < pos; i++)
		Log(tmp[i]);
	
	pos = 0;
	rxBuffPointer = tmp[0];

	return status;
}*/

#ifdef __cplusplus
extern "C"
#endif

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	if (txBuffer != '\0') {
		if (txPos == BUFF_SIZE - 1)
			txPos = 0;
		else
			txPos++;

		if (txPos != 0)
			buffer[txPos - 1] = '\0';
		else
			buffer[BUFF_SIZE - 1] = '\0';
	}


	txBuffer = buffer[txPos];
}