#include <stm32f4xx_hal.h>
#ifdef MPU6050
#include "MPU6050.h"
#endif // MPU6050
#ifdef MPU9250
#include "MPU9250.h"
#endif // MPU9250

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";
#ifdef MPU6050
MPU6050 mpu;
#endif // MPU6050
#ifdef MPU9250
MPU9250 mpu;
#endif // MPU9250



extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	mpu.dataReady = true;
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

	uint8_t result;
	if (result = mpu.Init()) {
		printf("Error %d\n", result);
	}

	long data[3];
	uint8_t accuracy;

	while (true)
	{
		HAL_IWDG_Refresh(&hiwdg);

		mpu.CheckNewData(data, &accuracy);

		if (mpu.CheckNewData(data, &accuracy))
			printf("data: %d %d %d\n", data[0], data[1], data[2]);
	}
}