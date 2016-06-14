/*
	This file contains the entry point (Reset_Handler) of your firmware project.
	The reset handled initializes the RAM and calls system library initializers as well as
	the platform-specific initializer and the main() function.
*/

extern void *_estack;

#define NULL ((void *)0)

void Reset_Handler();
void Default_Handler();

#ifdef DEBUG_DEFAULT_INTERRUPT_HANDLERS
void __attribute__ ((weak)) NMI_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void NMI_Handler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) HardFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void HardFault_Handler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) SVC_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SVC_Handler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) PendSV_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PendSV_Handler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) SysTick_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SysTick_Handler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) WWDG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void WWDG_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) PVD_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PVD_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) RTC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RTC_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) FLASH_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void FLASH_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) RCC_CRS_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RCC_CRS_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) EXTI0_1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI0_1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) EXTI2_3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI2_3_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) EXTI4_15_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTI4_15_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) TSC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TSC_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) DMA1_Channel1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Channel1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) DMA1_Channel2_3_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Channel2_3_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) DMA1_Channel4_5_6_7_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DMA1_Channel4_5_6_7_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) ADC1_COMP_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void ADC1_COMP_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) LPTIM1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void LPTIM1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) TIM2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM2_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) TIM6_DAC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM6_DAC_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) TIM21_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM21_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) TIM22_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM22_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) I2C1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) I2C2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void I2C2_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) SPI1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) SPI2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SPI2_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) USART1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) USART2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USART2_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) RNG_LPUART1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void RNG_LPUART1_IRQHandler();
	asm("bkpt 255");
};

void __attribute__ ((weak)) USB_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void USB_IRQHandler();
	asm("bkpt 255");
};

#else
void NMI_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void HardFault_Handler()               __attribute__ ((weak, alias ("Default_Handler")));
void SVC_Handler()                     __attribute__ ((weak, alias ("Default_Handler")));
void PendSV_Handler()                  __attribute__ ((weak, alias ("Default_Handler")));
void SysTick_Handler()                 __attribute__ ((weak, alias ("Default_Handler")));
void WWDG_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void PVD_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void RTC_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void FLASH_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void RCC_CRS_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI0_1_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI2_3_IRQHandler()              __attribute__ ((weak, alias ("Default_Handler")));
void EXTI4_15_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void TSC_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel1_IRQHandler()        __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel2_3_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void DMA1_Channel4_5_6_7_IRQHandler()  __attribute__ ((weak, alias ("Default_Handler")));
void ADC1_COMP_IRQHandler()            __attribute__ ((weak, alias ("Default_Handler")));
void LPTIM1_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void TIM2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void TIM6_DAC_IRQHandler()             __attribute__ ((weak, alias ("Default_Handler")));
void TIM21_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void TIM22_IRQHandler()                __attribute__ ((weak, alias ("Default_Handler")));
void I2C1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void I2C2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPI1_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void SPI2_IRQHandler()                 __attribute__ ((weak, alias ("Default_Handler")));
void USART1_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void USART2_IRQHandler()               __attribute__ ((weak, alias ("Default_Handler")));
void RNG_LPUART1_IRQHandler()          __attribute__ ((weak, alias ("Default_Handler")));
void USB_IRQHandler()                  __attribute__ ((weak, alias ("Default_Handler")));
#endif

void * g_pfnVectors[0x30] __attribute__ ((section (".isr_vector"))) = 
{
	&_estack,
	&Reset_Handler,
	&NMI_Handler,
	&HardFault_Handler,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	&SVC_Handler,
	NULL,
	NULL,
	&PendSV_Handler,
	&SysTick_Handler,
	&WWDG_IRQHandler,
	&PVD_IRQHandler,
	&RTC_IRQHandler,
	&FLASH_IRQHandler,
	&RCC_CRS_IRQHandler,
	&EXTI0_1_IRQHandler,
	&EXTI2_3_IRQHandler,
	&EXTI4_15_IRQHandler,
	&TSC_IRQHandler,
	&DMA1_Channel1_IRQHandler,
	&DMA1_Channel2_3_IRQHandler,
	&DMA1_Channel4_5_6_7_IRQHandler,
	&ADC1_COMP_IRQHandler,
	&LPTIM1_IRQHandler,
	NULL,
	&TIM2_IRQHandler,
	NULL,
	&TIM6_DAC_IRQHandler,
	NULL,
	NULL,
	&TIM21_IRQHandler,
	NULL,
	&TIM22_IRQHandler,
	&I2C1_IRQHandler,
	&I2C2_IRQHandler,
	&SPI1_IRQHandler,
	&SPI2_IRQHandler,
	&USART1_IRQHandler,
	&USART2_IRQHandler,
	&RNG_LPUART1_IRQHandler,
	NULL,
	&USB_IRQHandler,
};

void SystemInit();
void __libc_init_array();
int main();

extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

void __attribute__((naked, noreturn)) Reset_Handler()
{
	//Normally the CPU should will setup the based on the value from the first entry in the vector table.
	//If you encounter problems with accessing stack variables during initialization, ensure 
	//asm ("ldr sp, =_estack");

	void **pSource, **pDest;
	for (pSource = &_sidata, pDest = &_sdata; pDest != &_edata; pSource++, pDest++)
		*pDest = *pSource;

	for (pDest = &_sbss; pDest != &_ebss; pDest++)
		*pDest = 0;

	SystemInit();
	__libc_init_array();
	(void)main();
	for (;;) ;
}

void __attribute__((naked, noreturn)) Default_Handler()
{
	//If you get stuck here, your code is missing a handler for some interrupt.
	//Define a 'DEBUG_DEFAULT_INTERRUPT_HANDLERS' macro via VisualGDB Project Properties and rebuild your project.
	//This will pinpoint a specific missing vector.
	for (;;) ;
}
