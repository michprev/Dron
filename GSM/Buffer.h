#ifndef __BUFFER_H
#define __BUFFER_H

#include <stdlib.h>
#include <stdint.h>
#include <cstring>
#include <stm32f4xx_hal.h>

enum WaitFlag {WAIT_OK, WAIT_AT, WAIT_FINISH};
enum CommandFlag {COMMAND_AT, COMMAND_NOCOMMAND};

class Buffer
{
private:
	uint8_t *data;
	uint32_t size;
	uint32_t readPos;
	uint32_t writePos;
	const char delimeter = '\n';
	WaitFlag waitFlag;
	CommandFlag commandFlag;
	bool output;

	uint32_t getFreeSize();
	uint32_t getFullSize();
	uint32_t findLineFeed();
	bool checkInputSymbol();
	uint8_t readData(char *data, uint8_t count);
	void processData();

public:
	Buffer(uint32_t size);

	bool smsReady;
	bool callReady;
	bool ready;

	void(*SMS_Callback)();
	void(*Call_Callback)();

	void WriteByte(uint8_t *data);
	void WaitReady();
	void Init(UART_HandleTypeDef *huart);
	void Print();
	void TestSMS(UART_HandleTypeDef *huart);
};

#endif // !__BUFFER_H