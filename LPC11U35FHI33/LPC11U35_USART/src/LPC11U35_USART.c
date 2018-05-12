/*
 ===============================================================================
 Name        : LPC11U35_USART.c
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
#include "uart.h"


int main(void) {
	UART_Initialize(9600);
	UART_Send((uint8_t *) "Hello, UARTworld!\n", 18);
	while (1) {
	}
	return 0;
}
