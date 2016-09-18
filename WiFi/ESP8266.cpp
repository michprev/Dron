#include "ESP8266.h"


// free to write
uint32_t ESP8266::getFreeSize()
{
	uint32_t read = this->readPos;
	uint32_t write = this->writePos;

	if (read == write)
		return 0;

	if (read > write)
		return (read - write);

	if (write > read)
		return (this->size - write + read);
}

uint32_t ESP8266::getFullSize()
{
	uint32_t read = this->readPos;
	uint32_t write = this->writePos;

	if (read == write)
		return 0;

	if (write > read)
		return (write - read);

	if (read > write)
		return (this->size - read + write);	
}

//uint32_t Buffer::findLineFeed()
//{
//	uint32_t readPos = this->readPos;
//	uint32_t length = getFullSize();
//
//	while (length > 0) {
//		if (readPos >= this->size)
//			readPos = 0;
//
//		if (this->data[readPos] == '\n')
//			return readPos;
//
//		length--;
//		readPos++;
//	}
//
//	return -1;
//}

uint32_t ESP8266::findString(char * str)
{
	uint32_t length = strlen(str);
	uint32_t size = getFullSize();
	uint32_t retval = 0;
	uint32_t read = this->readPos;

	if (size < length)
		return -1;

	while (size > (length - 1)) {
		bool found = true;
		uint32_t tmpRead = read;

		if (read >= this->size)
			read = 0;

		for (uint32_t i = 0; i < length; i++) {
			if (tmpRead >= this->size)
				tmpRead = 0;

			if (this->data[tmpRead + i] != str[i]) {
				found = false;
				break;
			}
		}

		if (found)
			return retval;

		read++;
		retval++;
		size--;
	}

	return -1;
}

//bool Buffer::checkInputSymbol()
//{
//	uint32_t size = getFullSize();
//
//	if (size >= 2) {
//		if (this->readPos + 1 == this->size) {
//			if (this->data[this->readPos] == '>' && this->data[0] == ' ')
//				return true;
//		}
//		else {
//			if (this->data[this->readPos] == '>' && this->data[this->readPos + 1] == ' ')
//				return true;
//		}
//	}
//
//	return false;
//}

uint8_t ESP8266::readData(char *data, uint8_t count)
{
	uint32_t fullSize = getFullSize();

	if (fullSize == 0)
		return 0;
	if (fullSize < count)
		count = fullSize;

	if (count < this->size - this->readPos) {
		if (data != NULL)
			memcpy(data, &this->data[this->readPos], count);
		this->readPos += count;
		
		if (this->readPos == this->size)
			this->readPos = 0;

		return count;
	}
	else {
		uint8_t toCopy = count < this->size - count ? count : this->size - count;

		if (data != NULL) {
			memcpy(data, &this->data[this->readPos], toCopy);
			memcpy(&data[toCopy], this->data, count - toCopy);
		}
		this->readPos = count - toCopy;

		return count;
	}
}

void ESP8266::processData(bool printError)
{
	const uint16_t TMP_BUFFER_SIZE = 256;
	char buffer[256];

	uint32_t fullSize = getFullSize();
	uint32_t freeSize = getFreeSize();

	if (fullSize == 0)	// buffer empty
		return;

	//if (freeSize != 0 &&				// buffer is not full
	//	findString("\n") == -1 &&			// there is no \n
	//	fullSize < TMP_BUFFER_SIZE)
	//	return;

	if (findString("+IPD") == 0) {
		this->inIPD = true;
	}
	else if (findString("> ") == 0) {
		readData(NULL, 2);
		this->waitFlag = WAIT_OK;
		return;
	}

	while (findString("\n") != -1 && !inIPD) {
		uint8_t i = 0;
		uint8_t count;
		char c;

		if (findString("+IPD") == 0) {
			this->inIPD = true;
			break;
		}
		else if (findString("> ") == 0) {
			readData(NULL, 2);
			this->waitFlag = WAIT_OK;
			return;
		}

		do {
			count = readData(&c, 1);

			if (count == 1) {
				buffer[i] = c;
				i++;
			}
		} while (count == 1 && c != '\n');

		buffer[i] = '\0';


		if (strcmp("ready\r\n", buffer) == 0) {
			this->ready = true;
		}
		else if (strcmp(",CONNECT\r\n", buffer + 1) == 0) {
			this->LinkID = buffer[0];
		}
		else if (strcmp(",CLOSED\r\n", buffer + 1) == 0) {
			this->LinkID = -1;
		}
		else if (strcmp("ERROR\r\n", buffer) == 0) {
			uint32_t tmpPos = this->readPos;
			char tmp[101];
			
			if (tmpPos < 99) {
				uint8_t firstPart = 100 - tmpPos;
				strncpy(tmp, (char*)(this->data + this->size - firstPart), firstPart);
				strncpy(tmp + firstPart, (char*)this->data, tmpPos);
			}
			else {
				strncpy(tmp, (char*)(data + readPos - 100), 100);
			}

			tmp[100] = 0;
			this->error = tmp;
			/*if (printError)
				printf("error: %s\n", tmp);*/

			this->waitFlag = WAIT_ERROR;

			return;
		}
		else if (strcmp("OK\r\n", buffer) == 0) {
			this->waitFlag = WAIT_OK;
		}
		else if (strcmp("SEND OK\r\n", buffer) == 0) {
			this->waitFlag = WAIT_OK;
		}
		else if (strcmp("\r\n", buffer) == 0) {
			// do nothing
		}
		else if (strncmp("Recv", buffer, 4) == 0) {

		}
		else if (this->output)
			printf("Msg: %s\n",	buffer);
	}

	if (this->inIPD) {
		char lengthBuffer[10] = { '\0' };
		uint8_t pos = 0;
		char c;
		uint8_t readCount;
		uint8_t commaCount = 0;

		uint32_t tick = HAL_GetTick();

		do {
			readCount = readData(&c, 1);

			if (readCount == 1) { // data read
				if (commaCount == 2 && c != ':') {
					lengthBuffer[pos] = c;
					pos++;
				}

				if (c == ',')
					commaCount++;
			}
		} while (c != ':' && HAL_GetTick() - tick < 7000);

		if (c != ':')
			printf("IPD header timed out\n");

		int IPD_Length = atoi(lengthBuffer);
		int dataRead = 0;
		int IPD_Pos = 0;

		tick = HAL_GetTick();

		while (dataRead != IPD_Length && HAL_GetTick() - tick < 7000) {
			readCount = readData(&c, 1);

			if (readCount == 1) {
				this->IPD_Data[IPD_Pos] = c;
				dataRead++;
				IPD_Pos++;
			}
		}

		if (dataRead != IPD_Length)
			printf("IPD timed out\n");
		
		this->IPD_Data[IPD_Pos] = '\0';

		this->inIPD = false;

		//printf("IPD\n");

		if (this->IPD_Callback != NULL) {
			this->IPD_Callback(this->IPD_Data);
		}

		//printf("IPD returned\n");
	}
}

HAL_StatusTypeDef ESP8266::Send(char *str, bool wait, bool printError)
{
	this->lastCommand = str;
	HAL_StatusTypeDef s = HAL_UART_Transmit(this->huart, (uint8_t*)str, strlen(str), 1000);

	if (s != HAL_OK)
		printf("UART error\n");

	if (wait) {
		HAL_StatusTypeDef status = WaitReady(5000, printError);

		if (status == HAL_ERROR && printError)
			printf("error on: %s\n", this->error);
		else if (status == HAL_TIMEOUT && printError)
			printf("timed out: %s\n");

		return status;
	}
}

HAL_StatusTypeDef ESP8266::Send(char * data, uint16_t count, bool wait, bool printError)
{
	this->lastCommand = data;
	HAL_StatusTypeDef s = HAL_UART_Transmit(this->huart, (uint8_t*)data, count, 1000);

	if (s != HAL_OK)
		printf("UART error\n");

	if (wait) {
		HAL_StatusTypeDef status = WaitReady(5000, printError);

		if (status == HAL_ERROR && printError)
			printf("error on: %s\n", this->error);
		else if (status == HAL_TIMEOUT && printError)
			printf("timed out: %s\n");

		return status;
	}
}

ESP8266::ESP8266(UART_HandleTypeDef *huart, uint32_t size)
{
	this->readPos = 0;
	this->writePos = 0;
	this->size = size;
	this->data = (uint8_t*)calloc(size, sizeof(uint8_t));
	this->output = false;
	this->ready = false;
	this->inIPD = false;
	this->huart = huart;
	this->IPD_Callback = NULL;
	this->LinkID = -1;
}

void ESP8266::WriteByte(uint8_t * data)
{
	if (*data != '\0') {
		this->data[writePos] = *data;
		this->writePos++;

		if (this->writePos == this->size)
			this->writePos = 0;
	}
}

HAL_StatusTypeDef ESP8266::WaitReady(uint16_t delay, bool printError)
{
	this->waitFlag = WAIT_AT;
	uint32_t tick = HAL_GetTick();

	while (this->waitFlag == WAIT_AT) {
		processData(printError);

		if (HAL_GetTick() - tick > delay) {
			return HAL_TIMEOUT;
		}
	}

	if (this->waitFlag == WAIT_ERROR)
		return HAL_ERROR;

	return HAL_OK;
}

void ESP8266::Init()
{
	//Send("AT+GMR\r\n");
	Send("ATE0\r\n");
	Send("AT+CWMODE_CUR=2\r\n");
	Send("AT+CWSAP_CUR=\"DRON_WIFI\",\"123456789\",5,3,1,0\r\n");
	Send("AT+CWDHCP_CUR=0,1\r\n");
	//Send("AT+CIFSR\r\n");
	Send("AT+CIPMUX=1\r\n");
	Send("AT+CIPSERVER=1,80\r\n");
}
