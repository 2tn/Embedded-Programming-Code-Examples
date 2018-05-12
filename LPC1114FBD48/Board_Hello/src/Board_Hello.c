/*
 ===============================================================================
 Name        : Board_Hello.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
volatile static uint32_t count = 0;
void SysTick_Handler(void) {
	if (count == 1000) {
		LPC_GPIO3->DATA |= (1 << 3);
		count++;
	} else if (count == 2000) {
		LPC_GPIO3->DATA &= ~(1 << 3);
		count = 0;
	} else {
		count++;
	}

}
int main(void) {

	printf("Hello World\n");

	// Force the counter to be placed into memory

	volatile static int i;
	volatile static uint32_t period;
	period = SystemCoreClock / 1000;
	SysTick_Config(period);

	LPC_GPIO3->DIR |= (1 << 3);

	// Enter an infinite loop, just incrementing a counter
	while (1) {
		i++;
	}
	return 0;
}
