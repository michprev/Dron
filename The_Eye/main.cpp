#include <stm32f4xx_hal.h>
#include "../Barometer/MS5611.h"
#include "../Gyro/MPU6050.h"
#include "../WiFi_UDP/ESP8266_UDP.h"

const uint32_t UART_BUFFER_SIZE = 2048;
ESP8266_UDP esp8266 = ESP8266_UDP(UART_BUFFER_SIZE);
unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";
//MPU6050 mpu;

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	//mpu.dataReady = true;
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

void IPD_Callback(char *data) {
	//printf("%s\n", data);
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

	// reset gyro
	/*uint8_t result;
	if (result = mpu.Init()) {
		printf("Error %d\n", result);
	}
	else
		printf("Everything ok..\n");*/

	// reset barometer
	MS5611 ms5611;
	ms5611.Init();

	// reset ESP8266
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_Delay(250);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_SET);

	while (!esp8266.ready)
		esp8266.WaitReady();
	esp8266.IPD_Callback = &IPD_Callback;
	esp8266.output = true;
	esp8266.Init();
	printf("Init complete\n");
	

	int32_t press, temp;
	long data[3];
	uint8_t accuracy;
	uint8_t udpD[8];

	while (true) {
		HAL_IWDG_Refresh(&hiwdg);
		ms5611.GetData(&temp, &press);
		//mpu.CheckNewData(data, &accuracy);
		udpD[0] = temp & 0xFF;
		udpD[1] = (temp >> 8) & 0xFF;
		udpD[2] = (temp >> 16) & 0xFF;
		udpD[3] = (temp >> 24) & 0xFF;
		udpD[4] = press & 0xFF;
		udpD[5] = (press >> 8) & 0xFF;
		udpD[6] = (press >> 16) & 0xFF;
		udpD[7] = (press >> 24) & 0xFF;

		esp8266.SendUDP(udpD, 8);

		HAL_Delay(1000);
		esp8266.WaitReady(1);
	}
}
