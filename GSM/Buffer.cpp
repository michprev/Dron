#include "Buffer.h"


// free to write
uint32_t Buffer::getFreeSize()
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

uint32_t Buffer::getFullSize()
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

uint32_t Buffer::findLineFeed()
{
	uint32_t readPos = this->readPos;
	uint32_t length = getFullSize();

	while (length > 0) {
		if (readPos >= this->size)
			readPos = 0;

		if (this->data[readPos] == '\n')
			return readPos;

		length--;
		readPos++;
	}

	return -1;
}

bool Buffer::checkInputSymbol()
{
	uint32_t size = getFullSize();

	if (size >= 2) {
		if (this->readPos + 1 == this->size) {
			if (this->data[this->readPos] == '>' && this->data[0] == ' ')
				return true;
		}
		else {
			if (this->data[this->readPos] == '>' && this->data[this->readPos + 1] == ' ')
				return true;
		}
	}

	return false;
}

uint8_t Buffer::readData(char *data, uint8_t count)
{
	uint32_t fullSize = getFullSize();

	if (fullSize == 0)
		return 0;
	if (fullSize < count)
		count = fullSize;

	if (count < this->size - this->readPos) {
		memcpy(data, &this->data[this->readPos], count);
		this->readPos += count;
		
		if (this->readPos == this->size)
			this->readPos = 0;

		return count;
	}
	else {
		uint8_t toCopy = count < this->size - count ? count : this->size - count;

		memcpy(data, &this->data[this->readPos], toCopy);
		memcpy(&data[toCopy], this->data, count - toCopy);
		this->readPos = count - toCopy;

		return count;
	}
}

void Buffer::processData()
{
	const uint8_t TMP_BUFFER_SIZE = 256;
	char buffer[256];

	uint32_t fullSize = getFullSize();
	uint32_t freeSize = getFreeSize();

	if (fullSize == 0)	// buffer empty
		return;

	if (freeSize != 0 &&				// buffer is not full
		findLineFeed() == -1 &&			// there is no \n
		fullSize < TMP_BUFFER_SIZE)
		return;

	while (findLineFeed() != -1) {
		uint8_t i = 0;
		uint8_t count;
		char c;

		do {
			if (checkInputSymbol()) {
				printf("input symbol\n");
				this->waitFlag = WAIT_OK;
				readData(&c, 1);
				readData(&c, 1);
				break;
			}

			count = readData(&c, 1);

			if (count == 1)
				buffer[i] = c;

			i++;
		} while (count == 1 && c != '\n');

		buffer[i] = '\0';
		if (this->output)
			printf("Msg: %s\n", buffer);


		if (strcmp("SMS Ready\r\n", buffer) == 0) {
			this->smsReady = true;
			if (this->SMS_Callback != NULL)
				this->SMS_Callback();
		}
		else if (strcmp("Call Ready\r\n", buffer) == 0) {
			this->callReady = true;
			if (this->Call_Callback != NULL)
				this->Call_Callback();
		}
		else if (strcmp("RDY\r\n", buffer) == 0) {
			this->ready = true;
		}
		else if (strcmp("ERROR\r\n", buffer) == 0) {
			printf("error\n");
			printf("%s\n", this->data);
		}
		else if (strcmp("OK\r\n", buffer) == 0) {
			this->waitFlag = WAIT_FINISH;
		}
		/*if (this->output) {
			for (int k = 0; k < i; k++) {
				if (buffer[k] == '\r')
					printf("p: %d (\\r)\n", buffer[k]);
				else if (buffer[k] == '\n')
					printf("p: %d (\\n)\n", buffer[k]);
				else
					printf("p: %d (%c)\n", buffer[k], buffer[k]);
			}
			printf("end msg\n");
		}*/
	}

	if (checkInputSymbol()) {
		char c;
		this->waitFlag = WAIT_OK;
		readData(&c, 1);
		readData(&c, 1);
	}
}

Buffer::Buffer(uint32_t size)
{
	this->readPos = 0;
	this->writePos = 0;
	this->size = size;
	this->data = (uint8_t*)calloc(size, sizeof(uint8_t));
	this->output = false;
	this->smsReady = false;
	this->callReady = false;
	this->ready = false;
	this->SMS_Callback = NULL;
	this->Call_Callback = NULL;
}

void Buffer::WriteByte(uint8_t * data)
{
	if (*data != '\0') {
		this->data[writePos] = *data;
		this->writePos++;

		if (this->writePos == this->size)
			this->writePos = 0;
	}
}

void Buffer::WaitReady()
{
	this->waitFlag = WAIT_AT;
	uint32_t tick = HAL_GetTick();
	bool timedOut = false;

	while (this->waitFlag != WAIT_FINISH) {
		processData();

		if (HAL_GetTick() - tick > 3000) {
			timedOut = true;
			break;
		}

		/*if (this->output) {
			uint32_t length = getFullSize();

			for (int i = 0; i < length; i++) {
				if (data[this->readPos + i] == '\r')
					printf("d: %d (\\r)\n", data[this->readPos + i]);
				else if (data[this->readPos + i] == '\n')
					printf("d: %d (\\n)\n", data[this->readPos + i]);
				else
					printf("d: %d (%c)\n", data[this->readPos + i], data[this->readPos + i]);
			}
		}*/
	}

	if (timedOut) {
		printf("Timed out, read %d, write %d\n", this->readPos, this->writePos);
	}
}

void Buffer::Init(UART_HandleTypeDef *huart)
{
	this->output = false;

	char l[] = "ATE0\r\n";
	HAL_UART_Transmit(huart, (uint8_t*)l, strlen(l), 1000);
	WaitReady();

	char k[] = "AT\r\n";
	HAL_UART_Transmit(huart, (uint8_t*)k, strlen(k), 1000);
	WaitReady();

	char p[] = "AT+CSCS=?\r\n";
	HAL_UART_Transmit(huart, (uint8_t*)p, strlen(p), 1000);
	WaitReady();
}

void Buffer::Print()
{
	printf("%s\n", this->data);
}

void Buffer::TestSMS(UART_HandleTypeDef *huart)
{
	char mode[] = "AT+CMGF=1\r\n";
	HAL_UART_Transmit(huart, (uint8_t*)mode, strlen(mode), 1000);
	WaitReady();

	char number[] = "AT+CMGS=\"+420720246163\"\r";
	HAL_UART_Transmit(huart, (uint8_t*)number, strlen(number), 1000);
	WaitReady();

	char msg[] = "This is test message.\x1A\r";
	HAL_UART_Transmit(huart, (uint8_t*)msg, strlen(msg), 1000);
	WaitReady();
}
