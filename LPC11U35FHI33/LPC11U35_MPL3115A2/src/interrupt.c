/*
 * interrupt.c
 *
 *  Created on: 2016/09/04
 *      Author: Yusuke
 */

#include "interrupt.h"
#include "LPC11Uxx.h"


void FLEX_INT1_IRQHandler(void) {
	if (LPC_GPIO_PIN_INT->IST & (0x1 << 1)) {
		if (LPC_GPIO_PIN_INT->ISEL & (0x1 << 1)) {
		} else {
			if ((LPC_GPIO_PIN_INT->RISE & (0x1 << 1))
					&& (LPC_GPIO_PIN_INT->IENR & (0x1 << 1))) {
				LPC_GPIO_PIN_INT->RISE = 0x1 << 1;
				flag = 1;
			}
			LPC_GPIO_PIN_INT->IST = 0x1 << 1;
		}
	}
	return;
}

void interrupt_initialize(void) {
	LPC_SYSCON->SYSAHBCLKCTRL |= ((1 << 6) | (1 << 19) | (1 << 23) | (1 << 24));
	LPC_SYSCON->PINTSEL[1] = 16;
	LPC_GPIO_PIN_INT->ISEL &= ~(0x1 << 1); /* Edge trigger */
	LPC_GPIO_PIN_INT->IENR |= (0x1 << 1); /* Rising edge */
	LPC_GPIO_PIN_INT->SIENR |= (0x1 << 1); /* Rising edge */
	NVIC_EnableIRQ(FLEX_INT1_IRQn);
}
