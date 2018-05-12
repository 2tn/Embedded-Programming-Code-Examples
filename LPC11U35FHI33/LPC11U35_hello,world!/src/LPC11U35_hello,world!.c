/*
 ===============================================================================
 Name        : LPC11U35_hello,world!.c
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

#define LED1_ON		(LPC_GPIO->CLR[0] = (1 << 8))
#define LED1_OFF	(LPC_GPIO->SET[0] = (1 << 8))
#define LED2_ON		(LPC_GPIO->CLR[0] = (1 << 7))
#define LED2_OFF	(LPC_GPIO->SET[0] = (1 << 7))

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
volatile static long count = 0;
void SysTick_Handler(void) {
	count++;
	if (count == 1000) {
		LED1_ON;
		LED2_OFF;
		printf("LED1 ON / LED2 OFF\n");
		count = 0;
	} else if (count == 500) {
		LED1_OFF;
		LED2_ON;
		printf("LED1 OFF / LED2 ON\n");

	}
}

int main(void) {
	printf("Hello\n");
	SysTick_Config(SystemCoreClock / 1000);
	// TODO: insert code here
	// Force the counter to be placed into memory
	volatile static int i = 0;
	LPC_GPIO->DIR[0] |= (1 << 8);
	LPC_GPIO->DIR[0] |= (1 << 7);
	// Enter an infinite loop, just incrementing a counter
	while (1) {
		i++;
	}
	return 0;
}
