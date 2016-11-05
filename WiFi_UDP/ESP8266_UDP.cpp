#include "ESP8266_UDP.h"


HAL_StatusTypeDef ESP8266_UDP::UART_Init()
{
	if (__GPIOA_IS_CLK_DISABLED())
		__GPIOA_CLK_ENABLE();

	if (__USART1_IS_CLK_DISABLED())
		__USART1_CLK_ENABLE();

	if (__DMA2_IS_CLK_DISABLED())
		__DMA2_CLK_ENABLE();

	GPIO_InitTypeDef rst;
	rst.Pin = GPIO_PIN_4;
	rst.Mode = GPIO_MODE_OUTPUT_PP;
	rst.Pull = GPIO_PULLUP;;
	rst.Speed = GPIO_SPEED_HIGH;
	HAL_GPIO_Init(GPIOA, &rst);

	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	hdma_usart1_rx.Instance = DMA2_Stream2;
	hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
	hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
	hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
	hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
	hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&hdma_usart1_rx);

	__HAL_LINKDMA(&huart, hdmarx, hdma_usart1_rx);

	this->huart.Instance = USART1;
	this->huart.Init.BaudRate = 115200;
	this->huart.Init.WordLength = UART_WORDLENGTH_8B;
	this->huart.Init.StopBits = UART_STOPBITS_1;
	this->huart.Init.Parity = UART_PARITY_NONE;
	this->huart.Init.Mode = UART_MODE_TX_RX;
	this->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	this->huart.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&this->huart);

	HAL_UART_Receive_DMA(&this->huart, this->data, this->size);
}

uint32_t ESP8266_UDP::findString(char * str)
{
	uint32_t length = strlen(str);
	uint32_t retval = 0;
	uint32_t read = this->readPos;
	bool stop = false;

	do {
		bool found = true;
		uint32_t tmpRead = read;

		if (read >= this->size)
			read = 0;

		for (uint32_t i = 0; i < length; i++) {
			if (tmpRead >= this->size)
				tmpRead = 0;

			if (this->data[tmpRead + i] == '\0') {
				bool skip = false;
				for (uint8_t j = 0; j < 5; j++) {
					if (this->data[tmpRead + i + j] != '\0') {
						skip = true;
						break;
					}
				}

				if (!skip) {
					stop = true;
					found = false;
					break;
				}
			}

			if (this->data[tmpRead + i] != str[i]) {
				found = false;
				break;
			}
		}

		if (found)
			return retval;

		read++;
		retval++;
	} while (!stop);

	return -1;
}

uint8_t ESP8266_UDP::readByte(uint8_t *data, bool checkNull) {
	if (checkNull && this->data[this->readPos] == '\0')
		return 0;
	
	if (data != NULL)
		*data = this->data[this->readPos];
	this->data[this->readPos] = '\0';
	this->readPos++;

	if (this->readPos == this->size)
		this->readPos = 0;

	return 1;
}

void ESP8266_UDP::processData()
{
	char buffer[256];

	if (findString("+IPD") == 0) {
		this->inIPD = true;
	}
	else if (findString("> ") == 0) {
		readByte(NULL);
		readByte(NULL);
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
			readByte(NULL);
			readByte(NULL);
			this->waitFlag = WAIT_OK;
			return;
		}

		do {
			count = readByte((uint8_t*)&c);

			if (count == 1) {
				buffer[i] = c;
				i++;
			}
		} while (count == 1 && c != '\n');

		buffer[i] = '\0';


		if (strcmp("ready\r\n", buffer) == 0) {
			if (this->ready) {
				printf("%s\n", this->data);
				printf("restarted\n");
			}

			this->ready = true;
		}
		else if (strcmp("CONNECT\r\n", buffer) == 0) {

		}
		else if (strcmp("CLOSED\r\n", buffer) == 0) {
			printf("close\n");
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
		else if (strcmp("SEND FAIL\r\n", buffer) == 0) {
			this->clientPort = 0;
			this->clientIP[0] = '\0';
			this->handshaken = false;
		}
		else if (strcmp("\r\n", buffer) == 0) {

		}
		else if (strcmp("ATE0\r\r\n", buffer) == 0) {

		}
		else if (strncmp("Recv", buffer, 4) == 0) {

		}
		else if (strcmp(this->expectedResponse, buffer) == 0) {
			this->expectedResponse[0] = '\0';
			this->waitFlag = WAIT_OK;
		}
		else if (this->output)
			printf("Msg: %s\n", buffer);
	}

	if (this->inIPD) {
		char lengthBuffer[10] = { '\0' };
		uint8_t pos = 0;
		char c;
		uint8_t readCount;
		uint8_t commaCount = 0;

		char port[20] = { '\0' };

		do {
			readCount = readByte((uint8_t*)&c, true);

			if (readCount == 1) {
				if (commaCount == 1 && c != ',') {
					lengthBuffer[pos] = c;
					pos++;
				}
				else if (commaCount == 2 && c != ',') {
					this->clientIP[pos] = c;
					pos++;
				}
				else if (commaCount == 3 && c != ':') {
					port[pos] = c;
					pos++;
				}

				if (c == ',') {
					commaCount++;
					pos = 0;
				}
			}
		} while (c != ':');

		this->clientPort = atoi(port);
		this->handshaken = true;

		int IPD_Length = atoi(lengthBuffer);
		int dataRead = 0;
		int IPD_Pos = 0;

		while (dataRead != IPD_Length) {
			readCount = readByte((uint8_t*)&c, true);

			if (readCount == 1) {
				this->IPD_Data[IPD_Pos] = c;
				dataRead++;
				IPD_Pos++;
			}
		}

		this->inIPD = false;

		if (this->IPD_Callback != NULL) {
			this->IPD_Callback(this->IPD_Data, IPD_Length);
		}
	}
}

HAL_StatusTypeDef ESP8266_UDP::send(char *str)
{
	HAL_StatusTypeDef s = HAL_UART_Transmit(&this->huart, (uint8_t*)str, strlen(str), 5000);

	if (s != HAL_OK)
		printf("UART error\n");

	HAL_StatusTypeDef status = WaitReady(5000);

	if (status == HAL_ERROR)
		printf("error\n");
	if (status == HAL_TIMEOUT)
		printf("timed out\n");

	return status;
}

HAL_StatusTypeDef ESP8266_UDP::SendUDP(uint8_t *data, uint16_t length)
{
	if (this->handshaken) {
		char command[100];
		sprintf(command, "AT+CIPSEND=%d,\"%s\",%d\r\n", length, this->clientIP, this->clientPort);

		HAL_StatusTypeDef s = HAL_UART_Transmit(&this->huart, (uint8_t*)command, strlen(command), 5000);

		this->WaitReady(5000);

		s = HAL_UART_Transmit(&this->huart, data, length, 5000);

		if (s != HAL_OK)
			printf("UART error\n");

		HAL_StatusTypeDef status = WaitReady(5000);

		if (status == HAL_ERROR)
			printf("error\n");

		return status;
	}
}

ESP8266_UDP::ESP8266_UDP(uint32_t size)
{
	this->readPos = 0;
	this->size = size;
	this->data = (uint8_t*)calloc(size, sizeof(uint8_t));
	this->output = false;
	this->ready = false;
	this->inIPD = false;
	this->huart = huart;
	this->IPD_Callback = NULL;
	this->handshaken = false;

	UART_Init();
}

HAL_StatusTypeDef ESP8266_UDP::WaitReady(uint16_t delay)
{
	this->waitFlag = WAIT_AT;

	if (delay == 0) {
		processData();

		if (this->waitFlag == WAIT_AT)
			return HAL_TIMEOUT;
		if (this->waitFlag == WAIT_ERROR)
			return HAL_ERROR;
		return HAL_OK;
	}

	uint32_t tick = HAL_GetTick();

	while (this->waitFlag == WAIT_AT) {
		processData();

		if (HAL_GetTick() - tick > delay) {
			return HAL_TIMEOUT;
		}
	}

	if (this->waitFlag == WAIT_ERROR)
		return HAL_ERROR;
	else if (this->waitFlag == WAIT_AT)
		return HAL_TIMEOUT;

	return HAL_OK;
}

void ESP8266_UDP::Init()
{
	send("ATE0\r\n");
	send("AT+CWMODE_CUR=2\r\n");
	send("AT+CWSAP_CUR=\"DRON_WIFI\",\"123456789\",5,3,1,0\r\n");
	send("AT+CWDHCP_CUR=0,1\r\n");
	send("AT+CIPMUX=0\r\n");
	send("AT+CIPDINFO=1\r\n");
	send("AT+CIPSTART=\"UDP\",\"0\",0,4789,1\r\n");
	send("AT+CIPDINFO=1\r\n");
}