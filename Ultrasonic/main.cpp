#include <stm32f4xx_hal.h>

bool rising;
volatile bool update;
uint32_t start;
uint32_t time;
TIM_HandleTypeDef htim5;

extern "C" void EXTI1_IRQHandler(void)
{
	if (rising) {
		rising = false;
		start = htim5.Instance->CNT;
	}
	else {
		rising = true;
		uint32_t end = htim5.Instance->CNT;

		time = (end - start);
		update = true;
	}

	if (__HAL_GPIO_EXTI_GET_FLAG(GPIO_PIN_1) != RESET) {
		__HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
	}
}

uint32_t prev;

extern "C" void TIM5_IRQHandler(void)
{
	if (__HAL_TIM_GET_ITSTATUS(&htim5, TIM_IT_CC1) == SET) {
		printf("should not happen in our case\n");
	}

	// Channel 2 for HAL 1 ms tick
	if (__HAL_TIM_GET_ITSTATUS(&htim5, TIM_IT_CC2) == SET) {
		__HAL_TIM_CLEAR_IT(&htim5, TIM_IT_CC2);
		uint32_t val = __HAL_TIM_GetCounter(&htim5);
		if ((val - prev) >= 1000) {
			HAL_IncTick();
			// Prepare next interrupt
			__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, val + 1000);
			prev = val;
		}
		else {
			printf("should not happen\n");
		}
	}
}

extern "C" HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	uint32_t PclkFreq;

	// Get clock configuration
	// Note: PclkFreq contains here the Latency (not used after)
	HAL_RCC_GetClockConfig(&RCC_ClkInitStruct, &PclkFreq);

	// Get TIM5 clock value
	PclkFreq = HAL_RCC_GetPCLK1Freq();

	// Enable timer clock
	__TIM5_CLK_ENABLE();

	// Reset timer
	__TIM5_FORCE_RESET();
	__TIM5_RELEASE_RESET();

	// Configure time base
	htim5.Instance = TIM5;
	htim5.Init.Period = 0xFFFFFFFF;
	htim5.Init.Prescaler = (uint16_t)((PclkFreq) / 1000000) - 1; // 1 us tick
	htim5.Init.ClockDivision = RCC_HCLK_DIV1;
	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim5.Init.RepetitionCounter = 0;
	HAL_TIM_OC_Init(&htim5);

	HAL_NVIC_SetPriority(TIM5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(TIM5_IRQn);

	// Channel 1 for 1 us tick with no interrupt
	HAL_TIM_OC_Start(&htim5, TIM_CHANNEL_1);

	// Channel 2 for 1 us tick with interrupt every 1 ms
	HAL_TIM_OC_Start(&htim5, TIM_CHANNEL_2);
	prev = __HAL_TIM_GetCounter(&htim5);
	__HAL_TIM_SetCompare(&htim5, TIM_CHANNEL_2, prev + 1000);
	__HAL_TIM_ENABLE_IT(&htim5, TIM_IT_CC2);
}

void delay_us(uint32_t us) {
	uint32_t start = htim5.Instance->CNT;
	while (htim5.Instance->CNT - start < us);
}

int main(void)
{
	HAL_Init();

	__GPIOB_CLK_ENABLE();
	__GPIOC_CLK_ENABLE();

	GPIO_InitTypeDef trigg;
	trigg.Pin = GPIO_PIN_8;
	trigg.Mode = GPIO_MODE_OUTPUT_PP;
	trigg.Speed = GPIO_SPEED_HIGH;
	trigg.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOB, &trigg);

	GPIO_InitTypeDef echo;
	echo.Pin = GPIO_PIN_1;
	echo.Mode = GPIO_MODE_IT_RISING_FALLING;
	echo.Speed = GPIO_SPEED_HIGH;
	echo.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOC, &echo);

	HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI1_IRQn);


	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
	HAL_Delay(100);
	rising = true;
	update = false;

	while (true) {
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
		delay_us(10); // us
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);

		while (!update);
		update = false;

		printf("Distance %d cm\n", time * 331 / 10000 / 2);

		HAL_Delay(100); // ms
	}
}
