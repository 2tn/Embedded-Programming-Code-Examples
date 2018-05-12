/*
 * interrupt.h
 *
 *  Created on: 2016/09/04
 *      Author: Yusuke
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_
#include "LPC11Uxx.h"

extern uint8_t flag;

void FLEX_INT1_IRQHandler(void);
void interrupt_initialize(void);

#endif /* INTERRUPT_H_ */
