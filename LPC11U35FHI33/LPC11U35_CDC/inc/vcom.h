/*
 * vcom.h
 *
 *  Created on: 2016/07/02
 *      Author: Yusuke
 */

#ifndef VCOM_H_
#define VCOM_H_
#include "LPC11Uxx.h"

void vcom_send(uint8_t *data, uint8_t length);
void vcom_printf(const char* format, ...);
void vcom_init(void);

#endif /* VCOM_H_ */
