#ifndef __ESP8266_H
#define __ESP8266_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <stm32f4xx_hal.h>

enum WaitFlag {WAIT_AT, WAIT_OK, WAIT_ERROR};

class ESP8266
{
private:
	char expectedResponse[20] = { '\0' };
	char IPD_Data[1024];
	char sendBuffer[2048] = { '\0' };
	WaitFlag waitFlag;
	bool inIPD;
	char *lastCommand;
	bool output;
	uint32_t readPos;
	uint32_t writePos;
	int8_t LinkID;
	uint8_t *data;
	uint32_t size;

	HAL_StatusTypeDef UART_Init();
	uint32_t getFreeSize();
	uint32_t getFullSize();
	uint32_t findString(char *str);
	uint8_t readData(char *data, uint8_t count);
	void processData();
	HAL_StatusTypeDef send(char *str);
	HAL_StatusTypeDef send(char *data, uint16_t count);
	void sendPacket(char *command, char *data, uint16_t dataSize);

public:
	UART_HandleTypeDef huart;
	bool ready;
	bool canTimeout;
	void(*IPD_Callback)(char *data);

	ESP8266(uint32_t size);
	void SendFile(char *header, char *body, uint16_t bodySize);
	void WriteByte(uint8_t *data);
	HAL_StatusTypeDef WaitReady(uint16_t delay = 5000);
	void Init();
};

#endif