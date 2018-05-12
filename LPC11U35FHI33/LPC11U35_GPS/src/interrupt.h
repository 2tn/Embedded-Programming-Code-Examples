/*
 * interrupt.h
 *
 *  Created on: 2016/06/17
 *      Author: Yusuke
 */

#ifndef INTERRUPT_H_
#define INTERRUPT_H_

#include "LPC11Uxx.h"

void interrupt_initialize(void);

void Timer32_1_initialize(uint16_t ms);

void Timer32_1_start(void);
#endif /* INTERRUPT_H_ */
