/*
 ===============================================================================
 Name        : LPC11U35_ADC.c
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
#include "adc.h"

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

void ADC_initialize(void) {
	LPC_IOCON->PIO0_23 &= ~0x9F;
	LPC_IOCON->PIO0_23 |= 0x1;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 13);
	LPC_SYSCON->PDRUNCFG &= ~(1 << 4);
	LPC_ADC->CR = (SystemCoreClock / 4400000 - 1) << 8;
	LPC_ADC->CR |= (1 << 7);
}

uint16_t ADC_read() {
	LPC_ADC->CR |= ((1 << 24) | (1 << 7));
	while (!(LPC_ADC->DR7 & 0x80000000))
		;
	LPC_ADC->CR &= 0xF8FFFFFF;
	return (LPC_ADC->DR7 & 0x0000FFC0) >> 6;
}

int main(void) {
	//ADC_initialize();
	  ClearStats();
	ADCInit(4400000);
	while (1) {
		//printf("%ld\n", ADC_read());
		printf("%ld\n",ADCRead(7));
	}
	return 0;
}
