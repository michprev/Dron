#include "main.h"
#include "MPU6050.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

extern "C" int get_tick_count(unsigned long *count)
{
	count[0] = HAL_GetTick();
	return 0;
}

unsigned char *mpl_key = (unsigned char*)"eMPL 5.1";
cMPU6050 mpu;

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	mpu.dataReady = true;
}

int main(void)
{
	HAL_Init();
	
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
	else
		printf("Everything ok..\n");
	
	long data[3];
	uint8_t accuracy;

	while (true)
	{
		HAL_IWDG_Refresh(&hiwdg);


		mpu.CheckNewData(data, &accuracy);
	}
}