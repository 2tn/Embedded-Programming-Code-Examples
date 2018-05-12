/*
 ===============================================================================
 Name        : LPC11U35_Interrupt.c
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

//#define GROUP

#if defined GROUP
void GINT0_IRQHandler(void) {
	if ( LPC_GPIO_GROUP_INT0->CTRL & 0x1) {
		printf("int\n");
		LPC_GPIO_GROUP_INT0->CTRL |= 0x1;
	}
	return;
}
#else
void FLEX_INT0_IRQHandler(void) {
	if ( LPC_GPIO_PIN_INT->IST & (0x1 << 0)) {
		if ( LPC_GPIO_PIN_INT->ISEL & (0x1 << 0)) {
		} else {
			if (( LPC_GPIO_PIN_INT->RISE & (0x1 << 0))
					&& ( LPC_GPIO_PIN_INT->IENR & (0x1 << 0))) {
				LPC_GPIO_PIN_INT->RISE = 0x1 << 0;
				printf("RISE\n");
			}
			if (( LPC_GPIO_PIN_INT->FALL & (0x1 << 0))
					&& ( LPC_GPIO_PIN_INT->IENF & (0x1 << 0))) {
				LPC_GPIO_PIN_INT->FALL = 0x1 << 0;
				printf("FALL\n");
			}
			LPC_GPIO_PIN_INT->IST = 0x1 << 0;
		}
	}
	return;
}

#endif

int main(void) {
	/* Enable AHB clock to the GPIO domain. */
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 6);
	/* Enable AHB clock to the FlexInt, GroupedInt domain. */
	LPC_SYSCON->SYSAHBCLKCTRL |= ((1 << 19) | (1 << 23) | (1 << 24));

#if defined GROUP
	LPC_GPIO_GROUP_INT0->CTRL &= ~(0x1 << 2); /* Edge trigger */
	LPC_GPIO_GROUP_INT0->CTRL |= (0 << 1);
	LPC_GPIO_GROUP_INT0->PORT_POL[0] = (1 << 14);
	LPC_GPIO_GROUP_INT0->PORT_POL[1] = (0 << 15);
	LPC_GPIO_GROUP_INT0->PORT_ENA[0] = (1 << 14);
	LPC_GPIO_GROUP_INT0->PORT_ENA[1] = (1 << 15);
	/* as soon as enabled, an edge may be generated       */
	/* clear interrupt flag and NVIC pending interrupt to */
	/* workaround the potential edge generated as enabled */
	LPC_GPIO_GROUP_INT0->CTRL |= (1 << 0);
	NVIC_ClearPendingIRQ(GINT0_IRQn);
	NVIC_EnableIRQ(GINT0_IRQn);
#else
	LPC_SYSCON->PINTSEL[0] = 14;
	LPC_GPIO_PIN_INT->ISEL &= ~(0x1 << 0); /* Edge trigger */
	LPC_GPIO_PIN_INT->IENF |= (0x1 << 0); /* faling edge */
	LPC_GPIO_PIN_INT->SIENF |= (0x1 << 0); /* faling edge */
	LPC_GPIO_PIN_INT->IENR |= (0x1 << 0); /* Rising edge */
	LPC_GPIO_PIN_INT->SIENR |= (0x1 << 0); /* Rising edge */
	NVIC_EnableIRQ(FLEX_INT0_IRQn);
#endif
	while (1) {
	}
	return 0;
}
