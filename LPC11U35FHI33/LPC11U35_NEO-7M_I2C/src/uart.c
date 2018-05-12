/*
 * uart.c
 *
 *  Created on: 2016/06/17
 *      Author: Yusuke
 */

#include "uart.h"

void UART_Initialize(uint32_t baudrate) {
	uint32_t regVal;
	uint32_t Fdiv;

	NVIC_DisableIRQ (UART_IRQn);

	//①IOCONの設定
	LPC_IOCON->PIO0_18 = 0x01;	//set pin RXD
	LPC_IOCON->PIO0_19 = 0x01;	//set pin TXD

	//②USARTモジュールに送るクロックの設定
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);	//enable USART clock configuration
	LPC_SYSCON->UARTCLKDIV = 0x1;	//USART module frequency divider

	//③ボーレートの設定
	LPC_USART->LCR = ((1 << 7) | 0x03);	//DLAB=1にすることで、ボーレートの設定の変更を許可
	regVal = LPC_SYSCON->UARTCLKDIV;
	Fdiv = ((SystemCoreClock / regVal) / 16) / baudrate;
	LPC_USART->DLM = Fdiv / 256;
	LPC_USART->DLL = Fdiv % 256;
	LPC_USART->LCR = 0x03;	//DLAB=0にもどす

	//④FIFOにたまってるデータの削除
	while ((LPC_USART->LSR & 0x60) != 0x60)
		;
	while (LPC_USART->LSR & (1 << 1)) {
		regVal = LPC_USART->RBR; /* Dump data from RX FIFO */
	}

	//⑤割り込みの設定
	LPC_USART->FCR = 0x07;	//enable and reset TX and RX FIFO.
	LPC_USART->IER = 0x01;	//enable RBR interrupt register
	NVIC_EnableIRQ(UART_IRQn);
}

void UART_Send(uint8_t *BufferPtr, uint32_t Length) {

	while (Length != 0) {
		while (!(LPC_USART->LSR & (1 << 5)))
			;
		LPC_USART->THR = *BufferPtr;
		BufferPtr++;
		Length--;
	}
	return;
}


