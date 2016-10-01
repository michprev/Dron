#include <stm32f4xx_hal.h>
#include "Smoothie.h"
#include "../Barometer/MS5611.h"
#include "../WiFi/http_parser.h"
#include "../WiFi/ESP8266.h"

const uint32_t UART_BUFFER_SIZE = 2048;
http_parser parser;
http_parser_settings settings;
ESP8266 esp8266 = ESP8266(UART_BUFFER_SIZE);

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
	uint8_t data = esp8266.huart.Instance->DR & (uint8_t)0x00FF;

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

int main(void)
{
	HAL_Init();

	printf("init\n");

	IWDG_HandleTypeDef hiwdg;
	hiwdg.Instance = IWDG;
	hiwdg.Init.Prescaler = IWDG_PRESCALER_256;
	hiwdg.Init.Reload = 2047;

	if (HAL_IWDG_Init(&hiwdg) != HAL_OK)
	{
		printf("Could not start watchdog\n");
	}

	// reset barometer
	MS5611 ms5611;
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

		if (urlRead != urlWrite) {
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
				char body[50];

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
	}
}
