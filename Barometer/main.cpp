#include <stm32f4xx_hal.h>
#include "MS5611.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

int main(void)
{
	HAL_Init();

	MS5611 ms5611;
	ms5611.Init();

	int32_t press, temp;

	while (true) {
		ms5611.GetData(&temp, &press);

		printf("Temp: %d, press: %d\n", temp, press);

		HAL_Delay(100);
	}
}
