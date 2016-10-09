#include <stm32f4xx_hal.h>
#include "HMC5983.h"

extern "C" void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

int main(void)
{
	HAL_Init();

	HMC5983 hmc;
	hmc.Init();

	while (true) {

	}
}
