#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

uint64_t i = 0;

void SysTick_Handler(void) {
	i++;
}
void SysTick_initialize() {
	SysTick_Config(SystemCoreClock / 1000);
}
void get_ms(unsigned long *count) {
	*count = i;
}
void delay_ms(unsigned long time) {
	uint64_t begin = i;
	while (i <= begin + time)
		;
}

void Timer32_0_initialize(uint16_t frequency) {
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 9);
	LPC_CT32B0->MR0 = SystemCoreClock / frequency;
	LPC_CT32B0->MCR = 3;
	NVIC_EnableIRQ(TIMER_32_0_IRQn);
}
void Timer32_0_enable(void) {
	LPC_CT32B0->TCR = 1;
}

void Timer32_0_disable(void) {
	LPC_CT32B0->TCR = 2;
}
