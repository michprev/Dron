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

void ESP8266::processData()
{
	char buffer[256];

	uint32_t fullSize = getFullSize();
	uint32_t freeSize = getFreeSize();

	if (fullSize == 0)	// buffer empty
		return;

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


		char *commaPos = strchr(buffer, ',');

		if (strcmp("ready\r\n", buffer) == 0) {
			if (this->ready)
				printf("restarted\n");

			this->ready = true;
		}
		else if (strcmp(",CONNECT\r\n", buffer + 1) == 0) {
			this->LinkID = buffer[0];
		}
		else if (strcmp(",CLOSED\r\n", buffer + 1) == 0) {
			this->LinkID = -1;
		}
		else if (strcmp("ERROR\r\n", buffer) == 0) {
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
		else if (strcmp(this->expectedResponse, buffer) == 0) {
			this->expectedResponse[0] = '\0';
			this->waitFlag = WAIT_OK;
		}
		else if (commaPos != NULL && commaPos != buffer && commaPos != (buffer + strlen(buffer) - 1)) {
			uint32_t currentSegment = strtoul(buffer, NULL, 10);

			if (currentSegment != 0) {
				uint32_t previousSegment = strtoul(commaPos + 1, NULL, 10);

				if (previousSegment != 0) {
					sprintf(this->expectedResponse, "%c,%d,SEND OK\r\n", this->LinkID, currentSegment);
				}
				else if (previousSegment == 0 && currentSegment == 1) {
					sprintf(this->expectedResponse, "%c,%d,SEND OK\r\n", this->LinkID, currentSegment);
				}
			}
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
		} while (c != ':');

		if (c != ':')
			printf("IPD header timed out\n");

		int IPD_Length = atoi(lengthBuffer);
		int dataRead = 0;
		int IPD_Pos = 0;

		while (dataRead != IPD_Length) {
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
		
		if (this->IPD_Callback != NULL) {
			this->IPD_Callback(this->IPD_Data);
		}
	}
}

void ESP8266::sendPacket(char * command, char *data, uint16_t dataSize)
{
	if (this->LinkID != -1) {
		HAL_StatusTypeDef status;
		uint32_t tick = HAL_GetTick();

		do {
			HAL_Delay(20);
			if (HAL_GetTick() - tick > 7000) {
				printf("time out\n");
			}

			status = send(command);
		} while (status != HAL_OK);

		WaitReady();

		send(data, dataSize);
	}
}

void ESP8266::SendFile(char * header, char * body, uint16_t bodySize)
{
	uint16_t copied = 0;
	char command[30];
	sprintf(this->sendBuffer, "%s%d\r\n\r\n", header, bodySize);

	uint16_t headerSize = strlen(this->sendBuffer);

	if (bodySize != 0) {
		copied += (2048 - headerSize < bodySize ? 2048 - headerSize : bodySize);
		memcpy(this->sendBuffer + headerSize, body, copied);
	}

	sprintf(command, "AT+CIPSENDBUF=%c,%d\r\n", this->LinkID, headerSize + copied);
	this->sendPacket(command, this->sendBuffer, headerSize + copied);

	while (bodySize - copied >= 2048) {
		sprintf(command, "AT+CIPSENDBUF=%c,2048\r\n", this->LinkID);
		memcpy(this->sendBuffer, body + copied, 2048);

		this->sendPacket(command, this->sendBuffer, 2048);
		copied += 2048;
	}

	if (bodySize - copied != 0) {
		sprintf(command, "AT+CIPSENDBUF=%c,%d\r\n", this->LinkID, bodySize - copied);
		memcpy(this->sendBuffer, body + copied, bodySize - copied);

		this->sendPacket(command, this->sendBuffer, bodySize - copied);
	}
}

HAL_StatusTypeDef ESP8266::send(char *str)
{
	this->lastCommand = str;
	HAL_StatusTypeDef s = HAL_UART_Transmit(this->huart, (uint8_t*)str, strlen(str), 1000);

	if (s != HAL_OK)
		printf("UART error\n");

	HAL_StatusTypeDef status = WaitReady(5000);

	if (status == HAL_ERROR)
		printf("error\n");
	else if (status == HAL_TIMEOUT && !this->canTimeout)
		printf("timed out\n");

	return status;
}

HAL_StatusTypeDef ESP8266::send(char * data, uint16_t count)
{
	this->lastCommand = data;
	HAL_StatusTypeDef s = HAL_UART_Transmit(this->huart, (uint8_t*)data, count, 1000);

	if (s != HAL_OK)
		printf("UART error\n");

	HAL_StatusTypeDef status = WaitReady(5000);

	if (status == HAL_ERROR)
		printf("error\n");
	else if (status == HAL_TIMEOUT && !this->canTimeout)
		printf("timed out\n");

	return status;
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
	this->canTimeout = false;
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

HAL_StatusTypeDef ESP8266::WaitReady(uint16_t delay)
{
	this->waitFlag = WAIT_AT;
	uint32_t tick = HAL_GetTick();

	while (this->waitFlag == WAIT_AT) {
		processData();

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
	send("ATE0\r\n");
	send("AT+CWMODE_CUR=2\r\n");
	send("AT+CWSAP_CUR=\"DRON_WIFI\",\"123456789\",5,3,1,0\r\n");
	send("AT+CWDHCP_CUR=0,1\r\n");
	//Send("AT+CIFSR\r\n");
	send("AT+CIPMUX=1\r\n");
	send("AT+CIPSERVER=1,80\r\n");
}
