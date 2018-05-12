/*
 ===============================================================================
 Name        : LPC1114FN28_UARTtest.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

//This is echo-back program.
#define IER_RBR         (0x01<<0)
#define IER_THRE        (0x01<<1)
#define IER_RLS         (0x01<<2)
#define IER_ABEO        (0x01<<8)
#define IER_ABTO        (0x01<<9)

#define IIR_PEND        0x01
#define IIR_RLS         0x03
#define IIR_RDA         0x02
#define IIR_CTI         0x06
#define IIR_THRE        0x01
#define IIR_ABEO        (0x01<<8)
#define IIR_ABTO        (0x01<<9)

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#endif

#include <cr_section_macros.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
void UART_Configuration();
void UART_SendChar(char data);
void UART_SendString();
uint8_t UART_Read();

void UART_IRQHandler(void) {
	uint8_t IIRValue;
	IIRValue = LPC_UART->IIR;

	IIRValue >>= 1; /* skip pending bit in IIR */
	IIRValue &= 0x07; /* check bit 1~3, interrupt identification */
	if (IIRValue == IIR_RDA) /* Receive Data Available */
	{
		uint8_t Readdata;
		Readdata = UART_Read();
		UART_SendChar(Readdata);
	}
	return;
}

int main(void) {

	// TODO: insert code here
	// Force the counter to be placed into memory
	volatile static int i = 0;
	// Enter an infinite loop, just incrementing a counter
	UART_Configuration(115200);
	UART_SendString("TEST\n");
	while (1) {
		i++;
	}
	return 0;
}

void UART_Configuration(uint32_t baudrate) {
	uint32_t DL;
	LPC_IOCON->PIO3_0 |= 0x03;
	LPC_IOCON->PIO3_1 |= 0x03;
	LPC_IOCON->RXD_LOC |= 0x02;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 12);
	LPC_SYSCON->UARTCLKDIV = 0x01;
	DL = (SystemCoreClock * LPC_SYSCON->SYSAHBCLKDIV)
			/ (16 * baudrate * LPC_SYSCON->UARTCLKDIV);
	LPC_UART->LCR |= (1 << 7);
	LPC_UART->DLM = DL / 256;
	LPC_UART->DLL = DL % 256;
	LPC_UART->LCR &= ~(1 << 7);
	LPC_UART->LCR = 0x03;
	LPC_UART->FCR = 0x07;

	NVIC_EnableIRQ (UART_IRQn);
	LPC_UART->IER = IER_RBR | IER_RLS; /* Enable UART interrupt */
}

void UART_SendChar(char data) {
	while (!(LPC_UART->LSR & (1 << 5)))
		;
	LPC_UART->THR = data;
}

void UART_SendString(char *str) {
	while (*str != '\0') {
		UART_SendChar(*str);
		str++;
	}
}

uint8_t UART_Read() {
	while (!(LPC_UART->LSR & 0x01))
		;
	return LPC_UART->RBR;
}
