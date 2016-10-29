#ifndef __ESP8266_UDP_H
#define __ESP8266_UDP_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <stm32f4xx_hal.h>

enum WaitFlag { WAIT_AT, WAIT_OK, WAIT_ERROR };

class ESP8266_UDP
{
private:
	char expectedResponse[20] = { '\0' };
	char clientIP[16] = { '\0' };
	uint16_t clientPort;
	char IPD_Data[1024];
	char sendBuffer[2048] = { '\0' };
	WaitFlag waitFlag;
	bool inIPD;
	uint32_t readPos;
	uint32_t writePos;
	uint8_t *data;
	uint32_t size;

	HAL_StatusTypeDef UART_Init();
	uint32_t getFreeSize();
	uint32_t getFullSize();
	uint32_t findString(char *str);
	uint8_t readData(char *data, uint8_t count);
	void processData();
	HAL_StatusTypeDef send(char *);

public:
	UART_HandleTypeDef huart;
	bool ready;
	bool handshaken;
	bool output;
	void(*IPD_Callback)(char *data);

	ESP8266_UDP(uint32_t size);
	HAL_StatusTypeDef SendUDP(uint8_t *data, uint16_t length);
	void WriteByte(uint8_t *data);
	HAL_StatusTypeDef WaitReady(uint16_t delay = 5000);
	void Init();
	void ConnectToClient();
};

#endif