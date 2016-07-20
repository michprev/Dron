#include <stm32f4xx_hal.h>

#ifdef __cplusplus
extern "C"
#endif
void SysTick_Handler(void)
{
	HAL_IncTick();
	HAL_SYSTICK_IRQHandler();
}

uint32_t DELAY_Init(void) {
	CoreDebug->DEMCR &= ~0x01000000;
	CoreDebug->DEMCR |= 0x01000000;

	DWT->CTRL &= ~0x00000001;
	DWT->CTRL |= 0x00000001;

	DWT->CYCCNT = 0;

	uint32_t c = DWT->CYCCNT;

	__ASM volatile ("NOP");
	__ASM volatile ("NOP");

	return (DWT->CYCCNT - c);
}

static inline void Delay(__IO uint32_t us) {
	uint32_t start = DWT->CYCCNT;

	us *= (HAL_RCC_GetHCLKFreq() / 1000000);

	while ((DWT->CYCCNT - start) < us);
}

bool rising;
volatile bool update;
uint32_t start;
uint32_t time;

extern "C" void EXTI1_IRQHandler(void)
{
	if (rising) {
		rising = false;
		start = DWT->CYCCNT;
	}
	else {
		rising = true;
		uint32_t end = DWT->CYCCNT;

		time = (end - start) / (HAL_RCC_GetHCLKFreq() / 1000000);
		update = true;
	}

	if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_1) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	}
}

int main(void)
{
	HAL_Init();

	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef GPIO_InitStructure;
	GPIO_InitStructure.Pin = GPIO_PIN_8;
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStructure.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStructure.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitTypeDef exti;
	exti.Pin = GPIO_PIN_1;
	exti.Mode = GPIO_MODE_IT_RISING_FALLING;
	exti.Speed = GPIO_SPEED_HIGH;
	exti.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &exti);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);


	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(100);
	rising = true;
	update = false;

	if (!DELAY_Init())
		printf("Delay init error\n");

	while (true) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
		Delay(10); // us
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

		while (!update);
		update = false;

		printf("Distance %d cm\n", time * 331 / 10000 / 2);

		HAL_Delay(100); // ms
	}
}
