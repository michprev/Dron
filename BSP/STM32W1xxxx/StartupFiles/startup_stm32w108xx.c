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
}

void __attribute__ ((weak)) HardFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void HardFault_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) MemManage_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MemManage_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) BusFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BusFault_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) UsageFault_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void UsageFault_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) SVC_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SVC_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) DebugMon_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DebugMon_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) PendSV_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void PendSV_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) SysTick_Handler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SysTick_Handler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) TIM1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM1_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) TIM2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void TIM2_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) MNG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MNG_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) BASEBAND_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void BASEBAND_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) SLPTIM_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SLPTIM_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) SC1_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SC1_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) SC2_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SC2_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) SECURITY_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void SECURITY_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) MAC_TIM_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MAC_TIM_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) MAC_TR_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MAC_TR_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) MAC_RE_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void MAC_RE_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) ADC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void ADC_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) EXTIA_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTIA_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) EXTIB_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTIB_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) EXTIC_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTIC_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) EXTID_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void EXTID_IRQHandler();
	asm("bkpt 255");
}

void __attribute__ ((weak)) DEBUG_IRQHandler() 
{
	//If you hit the breakpoint below, one of the interrupts was unhandled in your code. 
	//Define the following function in your code to handle it:
	//	extern "C" void DEBUG_IRQHandler();
	asm("bkpt 255");
}

#else
void NMI_Handler()          __attribute__ ((weak, alias ("Default_Handler")));
void HardFault_Handler()    __attribute__ ((weak, alias ("Default_Handler")));
void MemManage_Handler()    __attribute__ ((weak, alias ("Default_Handler")));
void BusFault_Handler()     __attribute__ ((weak, alias ("Default_Handler")));
void UsageFault_Handler()   __attribute__ ((weak, alias ("Default_Handler")));
void SVC_Handler()          __attribute__ ((weak, alias ("Default_Handler")));
void DebugMon_Handler()     __attribute__ ((weak, alias ("Default_Handler")));
void PendSV_Handler()       __attribute__ ((weak, alias ("Default_Handler")));
void SysTick_Handler()      __attribute__ ((weak, alias ("Default_Handler")));
void TIM1_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void TIM2_IRQHandler()      __attribute__ ((weak, alias ("Default_Handler")));
void MNG_IRQHandler()       __attribute__ ((weak, alias ("Default_Handler")));
void BASEBAND_IRQHandler()  __attribute__ ((weak, alias ("Default_Handler")));
void SLPTIM_IRQHandler()    __attribute__ ((weak, alias ("Default_Handler")));
void SC1_IRQHandler()       __attribute__ ((weak, alias ("Default_Handler")));
void SC2_IRQHandler()       __attribute__ ((weak, alias ("Default_Handler")));
void SECURITY_IRQHandler()  __attribute__ ((weak, alias ("Default_Handler")));
void MAC_TIM_IRQHandler()   __attribute__ ((weak, alias ("Default_Handler")));
void MAC_TR_IRQHandler()    __attribute__ ((weak, alias ("Default_Handler")));
void MAC_RE_IRQHandler()    __attribute__ ((weak, alias ("Default_Handler")));
void ADC_IRQHandler()       __attribute__ ((weak, alias ("Default_Handler")));
void EXTIA_IRQHandler()     __attribute__ ((weak, alias ("Default_Handler")));
void EXTIB_IRQHandler()     __attribute__ ((weak, alias ("Default_Handler")));
void EXTIC_IRQHandler()     __attribute__ ((weak, alias ("Default_Handler")));
void EXTID_IRQHandler()     __attribute__ ((weak, alias ("Default_Handler")));
void DEBUG_IRQHandler()     __attribute__ ((weak, alias ("Default_Handler")));
#endif

void * g_pfnVectors[0x22] __attribute__ ((section (".isr_vector"))) = 
{
	&_estack,
	&Reset_Handler,
	&NMI_Handler,
	&HardFault_Handler,
	&MemManage_Handler,
	&BusFault_Handler,
	&UsageFault_Handler,
	NULL,
	NULL,
	NULL,
	NULL,
	&SVC_Handler,
	&DebugMon_Handler,
	NULL,
	&PendSV_Handler,
	&SysTick_Handler,
	&TIM1_IRQHandler,
	&TIM2_IRQHandler,
	&MNG_IRQHandler,
	&BASEBAND_IRQHandler,
	&SLPTIM_IRQHandler,
	&SC1_IRQHandler,
	&SC2_IRQHandler,
	&SECURITY_IRQHandler,
	&MAC_TIM_IRQHandler,
	&MAC_TR_IRQHandler,
	&MAC_RE_IRQHandler,
	&ADC_IRQHandler,
	&EXTIA_IRQHandler,
	&EXTIB_IRQHandler,
	&EXTIC_IRQHandler,
	&EXTID_IRQHandler,
	&DEBUG_IRQHandler,
	(void *)0xF108F85F /* BootRAM */,
};

void SystemInit();
void __libc_init_array();
int main();

extern void *_sidata, *_sdata, *_edata;
extern void *_sbss, *_ebss;

void __attribute__((naked, noreturn)) Reset_Handler()
{
	//Normally the CPU should will setup the based on the value from the first entry in the vector table.
	//If you encounter problems with accessing stack variables during initialization, ensure the line below is enabled.
	#ifdef sram_layout
	asm ("ldr sp, =_estack");
	#endif

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
