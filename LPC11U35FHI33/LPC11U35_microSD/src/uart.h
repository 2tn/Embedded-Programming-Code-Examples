/*
 * uart.h
 *
 *  Created on: 2016/06/17
 *      Author: Yusuke
 */

#ifndef UART_H_
#define UART_H_
#include <stdio.h>
#include <stdarg.h>

#include "LPC11Uxx.h"

void UART_Initialize(uint32_t baudrate);
void UART_Send(uint8_t *BufferPtr, uint32_t Length);
void UART_printf(const char* format, ...);
#endif /* UART_H_ */
