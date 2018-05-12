/*
 * interrupt.c
 *
 *  Created on: 2016/06/17
 *      Author: Yusuke
 */

#include "interrupt.h"
#include "uart.h"

void interrupt_initialize(void) {
	//PIO0_14
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 19);
	LPC_SYSCON->PINTSEL[0] = 2;
	LPC_GPIO_PIN_INT->ISEL &= ~(0x1 << 0); /* Edge trigger */
	//LPC_GPIO_PIN_INT->IENF |= (0x1 << 0); /* faling edge */
	//LPC_GPIO_PIN_INT->SIENF |= (0x1 << 0); /* faling edge */
	LPC_GPIO_PIN_INT->IENR |= (0x1 << 0); /* Rising edge */
	LPC_GPIO_PIN_INT->SIENR |= (0x1 << 0); /* Rising edge */
	NVIC_EnableIRQ(FLEX_INT0_IRQn);
}

void Timer32_1_initialize(uint16_t ms) {
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 10);
	LPC_CT32B1->MR0 = (SystemCoreClock / 1000 / 2) * ms;
	LPC_CT32B1->MCR = 7;
	NVIC_EnableIRQ(TIMER_32_1_IRQn);
}

void Timer32_1_start() {
	LPC_CT32B1->TC = 0;
	LPC_CT32B1->TCR = 1;
}
