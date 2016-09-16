#ifndef __BUFFER_H
#define __BUFFER_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <stm32f4xx_hal.h>

enum WaitFlag {WAIT_AT, WAIT_OK, WAIT_ERROR};

class Buffer
{
private:
	char IPD_Data[1024];
	WaitFlag waitFlag;
	bool inIPD;
	UART_HandleTypeDef *huart;
	
	uint32_t getFreeSize();
	uint32_t getFullSize();
	//uint32_t findLineFeed();
	uint32_t findString(char *str);
	bool checkInputSymbol();
	uint8_t readData(char *data, uint8_t count);
	void processData();

public:
	Buffer(UART_HandleTypeDef *huart, uint32_t size);

	uint32_t readPos;
	uint32_t writePos;
	uint32_t size;
	uint8_t *data;
	char LinkID;
	bool ready;
	bool output;
	bool busy_s;
	void(*IPD_Callback)(char *data);

	HAL_StatusTypeDef Send(char *str, bool wait = true);
	HAL_StatusTypeDef Send(char *data, uint16_t count, bool wait = true);
	void WriteByte(uint8_t *data);
	HAL_StatusTypeDef WaitReady(uint16_t delay = 5000);
	void Init();
};

#endif // !__BUFFER_H