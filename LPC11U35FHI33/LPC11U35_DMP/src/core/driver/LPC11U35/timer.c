#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

uint64_t i = 0;

void SysTick_Handler(void) {
	i++;
}
void timer_initialize() {
	SysTick_Config(SystemCoreClock / 1000);
}
void get_ms(unsigned long *count) {
	*count = i;
	return 0;
}
void delay_ms(unsigned long time) {
	uint64_t begin = i;
	while (i <= begin + time)
		;
	return 0;
}
