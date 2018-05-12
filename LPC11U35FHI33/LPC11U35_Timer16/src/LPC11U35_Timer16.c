/*
 ===============================================================================
 Name        : LPC11U35_Timer16.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

static int count = 0;
void SysTick_Handler(void) {
	if (count == 30) {
		Timer16_0_enable();
		printf("3sec\n");
		count = 0;
	} else {
		count++;
	}
}

void Timer16_0_initialize(uint16_t frequency) {
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 7);
	LPC_CT16B0->MR0 = 1000 / frequency;
	LPC_CT16B0->MCR = 7;
	LPC_CT16B0->PR = SystemCoreClock / 1000;
	NVIC_EnableIRQ(TIMER_16_0_IRQn);
}
void Timer16_0_enable(void) {
	LPC_CT16B0->TCR = 2;
	LPC_CT16B0->TCR = 1;
}

void Timer16_0_disable(void) {
	LPC_CT16B0->TCR = 0;
}

void TIMER16_0_IRQHandler(void) {
	if ( LPC_CT16B0->IR & (0x01 << 0)) {
		LPC_CT16B0->IR = 0x1 << 0;
		printf("interrupt\n");
	}
}

int main(void) {
	SysTick_Config(SystemCoreClock / 10);
	Timer16_0_initialize(10);
	Timer16_0_enable();

	while (1) {
	}
	return 0;
}
