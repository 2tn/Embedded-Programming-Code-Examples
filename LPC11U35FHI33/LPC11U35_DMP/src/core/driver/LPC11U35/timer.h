/*
 * timer.h
 *
 *  Created on: 2016/05/22
 *      Author: Yusuke
 */

#ifndef CORE_DRIVER_LPC11U35_TIMER_H_
#define CORE_DRIVER_LPC11U35_TIMER_H_

void timer_initialize();
void get_ms(unsigned long *count);
void delay_ms(unsigned long time);

#endif /* CORE_DRIVER_LPC11U35_TIMER_H_ */
