#ifndef __NEO_M8N_H
#define __NEO_M8N_H

#include <stm32f4xx_hal.h>
#include <stdlib.h>
#include <string.h>

class NEO_M8N
{
private:
	uint32_t readPos;
	
	uint32_t size;

	HAL_StatusTypeDef UART_Init();
	uint32_t getFreeSize();
	uint32_t getFullSize();
	uint32_t findString(char * str);
	uint8_t readData(char *data, uint8_t count);
	void processMessage(char *msg, uint8_t len);
public:
	UART_HandleTypeDef huart;
	DMA_HandleTypeDef hdma_usart3_rx;
	uint8_t *data;
	uint32_t writePos;

	NEO_M8N(uint32_t size);
	void Init();
	void WriteByte(uint8_t *data);
	void ParseData();
};

#endif