#include "NEO_M8N.h"


HAL_StatusTypeDef NEO_M8N::UART_Init()
{
	if (__GPIOA_IS_CLK_DISABLED())
		__GPIOA_CLK_ENABLE();

	if (__USART1_IS_CLK_DISABLED())
		__USART1_CLK_ENABLE();

	if (__DMA2_IS_CLK_DISABLED())
		__DMA2_CLK_ENABLE();

	/*USART1 GPIO Configuration
	PA9      ------> USART1_TX
	PA10     ------> USART1_RX
	*/
	GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_PIN_9 | GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART1;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	this->hdma_usart1_rx.Instance = DMA2_Stream2;
	this->hdma_usart1_rx.Init.Channel = DMA_CHANNEL_4;
	this->hdma_usart1_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
	this->hdma_usart1_rx.Init.PeriphInc = DMA_PINC_DISABLE;
	this->hdma_usart1_rx.Init.MemInc = DMA_MINC_ENABLE;
	this->hdma_usart1_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
	this->hdma_usart1_rx.Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
	this->hdma_usart1_rx.Init.Mode = DMA_CIRCULAR;
	this->hdma_usart1_rx.Init.Priority = DMA_PRIORITY_LOW;
	this->hdma_usart1_rx.Init.FIFOMode = DMA_FIFOMODE_DISABLE;
	HAL_DMA_Init(&this->hdma_usart1_rx);

	this->huart.Instance = USART1;
	this->huart.Init.BaudRate = 9600;
	this->huart.Init.WordLength = UART_WORDLENGTH_8B;
	this->huart.Init.StopBits = UART_STOPBITS_1;
	this->huart.Init.Parity = UART_PARITY_NONE;
	this->huart.Init.Mode = UART_MODE_TX_RX;
	this->huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	this->huart.Init.OverSampling = UART_OVERSAMPLING_16;
	HAL_UART_Init(&this->huart);

	__HAL_LINKDMA(&huart, hdmarx, this->hdma_usart1_rx);

	HAL_NVIC_SetPriority(DMA2_Stream2_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA2_Stream2_IRQn);

	//__HAL_UART_ENABLE_IT(&this->huart, UART_IT_RXNE);

	/* Peripheral interrupt init */
	/*HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART3_IRQn);*/
}

NEO_M8N::NEO_M8N(uint32_t size)
{
	this->readPos = 0;
	this->writePos = 0;
	this->size = size;
	this->data = (uint8_t*)calloc(size, sizeof(uint8_t));
}

// free to write
uint32_t NEO_M8N::getFreeSize()
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

uint32_t NEO_M8N::getFullSize()
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

void NEO_M8N::Init()
{
	UART_Init();
}

void NEO_M8N::WriteByte(uint8_t * data)
{
	if (*data != '\0') {
		this->data[writePos] = *data;
		this->writePos++;

		if (this->writePos == this->size)
			this->writePos = 0;
	}
}

uint32_t NEO_M8N::findString(char * str)
{
	uint32_t length = strlen(str);
	uint32_t size = getFullSize();
	uint32_t retval = 0;
	uint32_t read = this->readPos;

	if (size < length)
		return -1;

	while (size >(length - 1)) {
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

uint8_t NEO_M8N::readData(char *data, uint8_t count)
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

void NEO_M8N::processMessage(char *msg, uint8_t len)
{
	if (len < 6)
		return;

	// check checksum
	if (msg[len - 5] != '*')
		return;

	uint8_t sum = 0;
	
	if (msg[len - 3] >= '0' && msg[len - 3] <= '9')
		sum += msg[len - 3] - '0';
	else if (msg[len - 3] >= 'A' && msg[len - 3] <= 'F')
		sum += msg[len - 3] - 'A' + 10;
	else if (msg[len - 3] >= 'a' && msg[len - 3] <= 'f')
		sum += msg[len - 3] - 'a' + 10;

	if (msg[len - 4] >= '0' && msg[len - 4] <= '9')
		sum += (msg[len - 4] - '0') * 16;
	else if (msg[len - 4] >= 'A' && msg[len - 4] <= 'F')
		sum += (msg[len - 4] - 'A' + 10) * 16;
	else if (msg[len - 4] >= 'a' && msg[len - 4] <= 'f')
		sum += (msg[len - 4] - 'a' + 10) * 16;

	for (uint8_t i = 1; i < len - 5; i++)
		sum ^= msg[i];

	if (sum != 0)
		return;

	if (msg[0] != '$')
		return;

	if (strncmp("GP", msg + 1, 2) != 0 && strncmp("GL", msg + 1, 2) != 0 && strncmp("GA", msg + 1, 2) != 0 &&
		strncmp("GB", msg + 1, 2) != 0 && strncmp("GN", msg + 1, 2) != 0)
		return;

	// Datum Reference
	if (strncmp("DTM", msg + 3, 3) == 0) {

	}
	// GNSS Satellite Fault Detection
	else if (strncmp("GBS", msg + 3, 3) == 0) {

	}
	// Global positioning system fix data
	else if (strncmp("GGA", msg + 3, 3) == 0) {

	}
	// Latitude and longitude, with time of position fix and status
	else if (strncmp("GLL", msg + 3, 3) == 0) {
		printf("gll\n");
	}
	// GNSS fix data
	else if (strncmp("GNS", msg + 3, 3) == 0) {

	}
	// GNSS Range Residuals
	else if (strncmp("GRS", msg + 3, 3) == 0) {

	}
	// GNSS DOP and Active Satellites
	else if (strncmp("GSA", msg + 3, 3) == 0) {

	}
	// GNSS Pseudo Range Error Statistics
	else if (strncmp("GST", msg + 3, 3) == 0) {

	}
	// GNSS Satellites in View
	else if (strncmp("GSV", msg + 3, 3) == 0) {

	}
	// Recommended Minimum data
	else if (strncmp("RMC", msg + 3, 3) == 0) {

	}
	// Text Transmission
	else if (strncmp("TXT", msg + 3, 3) == 0) {

	}
	// Dual ground/water distance
	else if (strncmp("VLW", msg + 3, 3) == 0) {

	}
	// Course over ground and Ground speed
	else if (strncmp("VTG", msg + 3, 3) == 0) {

	}
	// Time and Date
	else if (strncmp("ZDA", msg + 3, 3) == 0) {
		printf("date\n");
	}
	else
		printf("Unsupported message\n");
}

void NEO_M8N::ParseData()
{
	char buffer[255];

	uint32_t fullSize = getFullSize();
	uint32_t freeSize = getFreeSize();

	if (fullSize == 0)	// buffer empty
		return;

	while (findString("\r\n") != -1) {
		uint8_t i = 0;
		uint8_t count;
		char c;

		do {
			count = readData(&c, 1);

			if (count == 1) {
				buffer[i] = c;
				i++;
			}
		} while (count == 1 && c != '\n');

		buffer[i] = '\0';

		processMessage(buffer, i);
	}
}
