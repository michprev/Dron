#include <stm32f4xx_hal.h>
#include "Smoothie.h"
#include "../Barometer/MS5611.h"
#include "../WiFi/http_parser.h"
#include "../WiFi/ESP8266.h"

I2C_HandleTypeDef hi2c;
const uint32_t UART_BUFFER_SIZE = 2048;
UART_HandleTypeDef huart;
http_parser parser;
http_parser_settings settings;
ESP8266 esp8266 = ESP8266(&huart, UART_BUFFER_SIZE);
IWDG_HandleTypeDef hiwdg;

char url[10][20];
uint8_t urlRead = 0;
uint8_t urlWrite = 0;

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

extern "C" void USART1_IRQHandler(void)
{
	uint8_t data = huart.Instance->DR & (uint8_t)0x00FF;

	esp8266.WriteByte(&data);
}

extern "C" void HardFault_Handler(void)
{
	printf("hard fault\n");
}

int on_url_callback(http_parser *parser, const char *at, size_t length) {
	strncpy(url[urlWrite], at, length);
	url[urlWrite][length] = '\0';

	urlWrite++;
	if (urlWrite == 10)
		urlWrite = 0;

	return 0;
}

void IPD_Callback(char *data) {
	uint16_t parsed = http_parser_execute(&parser, &settings, data, strlen(data));

	if (parsed != strlen(data)) {
		printf("IPD not parsed\n");
		printf("Parsed %d of %d\n", parsed, strlen(data));
	}
}

void I2cMaster_Init()
{
	if (__GPIOB_IS_CLK_DISABLED())
		__GPIOB_CLK_ENABLE();

	if (__I2C1_IS_CLK_DISABLED())
		__I2C1_CLK_ENABLE();

	GPIO_InitTypeDef gpio;
	gpio.Pin = GPIO_PIN_8 | GPIO_PIN_9;
	gpio.Mode = GPIO_MODE_AF_OD;
	gpio.Speed = GPIO_SPEED_MEDIUM;
	gpio.Pull = GPIO_PULLUP;
	gpio.Alternate = GPIO_AF4_I2C1;

	HAL_GPIO_Init(GPIOB, &gpio);

	hi2c.Instance = I2C1;
	hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c.Init.ClockSpeed = 100000;
	hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	hi2c.Init.OwnAddress1 = 0;
	hi2c.Init.OwnAddress2 = 0;

	HAL_I2C_Init(&hi2c);
}

void UART_Init() {
	if (__GPIOA_IS_CLK_DISABLED())
		__GPIOA_CLK_ENABLE();

	if (__USART1_IS_CLK_DISABLED())
		__USART1_CLK_ENABLE();

	GPIO_InitTypeDef rst;
	rst.Pin = GPIO_PIN_5;
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

	huart.Instance = USART1;
	huart.Init.BaudRate = 115200;
	huart.Init.WordLength = UART_WORDLENGTH_8B;
	huart.Init.StopBits = UART_STOPBITS_1;
	huart.Init.Parity = UART_PARITY_NONE;
	huart.Init.Mode = UART_MODE_TX_RX;
	huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart.Init.OverSampling = UART_OVERSAMPLING_8;
	HAL_UART_Init(&huart);

	__HAL_UART_ENABLE_IT(&huart, UART_IT_RXNE);

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
}

int main(void)
{
	HAL_Init();

	printf("init\n");

	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hiwdg.Init.Reload = 2047;

	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		printf("Could not start watchdog\n");
	}

	I2cMaster_Init();
	UART_Init();

	// reset barometer
	MS5611 ms5611 = MS5611(&hi2c);
	ms5611.Init();

	// reset ESP8266
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET);
	HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET);

	http_parser_init(&parser, HTTP_BOTH);
	settings.on_url = on_url_callback;

	while (!esp8266.ready)
		esp8266.WaitReady();
	esp8266.IPD_Callback = &IPD_Callback;
	esp8266.Init();
	printf("Init complete\n");
	

	int32_t press, temp;

	while (true) {
		HAL_IWDG_Refresh(&hiwdg);

		while (urlRead != urlWrite) {
			HAL_Delay(20);
			HAL_IWDG_Refresh(&hiwdg);

			if (strcmp(url[urlRead], "/") == 0) {
				char body[] = "<!DOCTYPE html> <html> <head> <title>DronUI</title> <meta charset=\"utf-8\" /> <script type=\"text/javascript\" src=\"smoothie.js\"></script> <script> function init() { var tempChart = new SmoothieChart({ interpolation: 'linear' }); var tempLine = new TimeSeries(); tempChart.addTimeSeries(tempLine, { lineWidth: 2, strokeStyle: '#00ff00' }); tempChart.streamTo(document.getElementById(\"tempCanvas\"), 1000); var pressChart = new SmoothieChart({ interpolation: 'linear' }); var pressLine = new TimeSeries(); pressChart.addTimeSeries(pressLine, { lineWidth: 2, strokeStyle: '#00ff00' }); pressChart.streamTo(document.getElementById(\"pressCanvas\"), 1000); setInterval(function () { var xhttp = new XMLHttpRequest(); xhttp.onreadystatechange = function () { if (this.readyState == 4 && this.status == 200) { var data = JSON.parse(this.responseText); tempLine.append(new Date().getTime(), data.temp); pressLine.append(new Date().getTime(), data.press); } }; xhttp.open(\"GET\", \"getData\", true); xhttp.send(); }, 1000); } </script> </head> <body onload=\"init()\"> <h2>Ultrasonic sensor</h2> <canvas id=\"tempCanvas\" width=\"900\" height=\"100\"></canvas> <canvas id=\"pressCanvas\" width=\"900\" height=\"100\"></canvas> </body> </html>";
				char header[] = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: ";

				esp8266.SendFile(header, body, strlen(body));
			}
			else if (strcmp(url[urlRead], "/smoothie.js") == 0) {
				char header[] = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: application/javascript\r\nContent-Encoding: gzip\r\nContent-Length: ";

				esp8266.SendFile(header, smoothie, smoothie_size);
			}
			else if (strcmp(url[urlRead], "/favicon.ico") == 0) {
				char header[] = "HTTP/1.1 404 Not Found\r\nConnection: keep-alive\r\nContent-Type: text/html\r\nContent-Length: ";

				esp8266.SendFile(header, NULL, 0);
			}
			else if (strcmp(url[urlRead], "/getData") == 0) {
				esp8266.canTimeout = true;
				char header[] = "HTTP/1.1 200 OK\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nContent-Length: ";
				char body[30];

				ms5611.GetData(&temp, &press);

				sprintf(body, "{\r\n\"temp\": %d,\r\n\"press\": %d\r\n}", temp, press);
				esp8266.SendFile(header, body, strlen(body));
			}
			else {
				printf("Unhandled URL: %s\n", url[urlRead]);
			}

			urlRead++;
			if (urlRead == 10)
				urlRead = 0;
		}


		esp8266.WaitReady(50);

		HAL_Delay(100);
	}
}
