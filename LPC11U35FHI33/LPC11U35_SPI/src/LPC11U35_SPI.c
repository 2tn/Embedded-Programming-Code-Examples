/*
 ===============================================================================
 Name        : LPC11U35_SPI.c
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
#define MPU9250_CS_ON	(LPC_GPIO->CLR[0] = (1 << 6))
#define MPU9250_CS_OFF	(LPC_GPIO->SET[0] = (1 << 6))

uint8_t dummy;
uint8_t SSP_Transfer(uint8_t data) {
	while (LPC_SSP1->SR & 0x10)
		;
	LPC_SSP1->DR = data;
	while (LPC_SSP1->SR & 0x10)
		;
	return LPC_SSP1->DR;
}

void SSP_Receive(uint8_t *data, uint32_t length) {
	uint16_t i = 0;
	for (i = 0; i < 1000; i++)
		;
	while (length != 0) {
		*data = SSP_Transfer(0xFF);
		data++;
		length--;
	}
	return;
}

void SSP_Send(uint8_t *data, uint32_t length) {
	while (length != 0) {
		dummy = SSP_Transfer(*data);
		data++;
		length--;
	}
	return;
}

void SSP_Initialize() {
	LPC_SYSCON->SYSAHBCLKCTRL |= ((1 << 18) | (1 << 6)); //enable SSP1&GPIO clock
	LPC_SYSCON->SSP1CLKDIV = 0x02; //set clock divider
	LPC_SYSCON->PRESETCTRL |= (1 << 2); //reset clock

	LPC_IOCON->PIO0_21 &= ~0x7;
	LPC_IOCON->PIO0_21 |= 0x2; //MOSI1
	LPC_IOCON->PIO0_22 &= ~0x7;
	LPC_IOCON->PIO0_22 |= 0x3; //MISO1
	LPC_IOCON->PIO1_15 &= ~0x7;
	LPC_IOCON->PIO1_15 |= 0x3; //SCK1
	LPC_IOCON->PIO0_6 &= ~0x7;
	LPC_IOCON->PIO0_6 = 0x0; //CS1

	LPC_GPIO->DIR[0] |= (1 << 6); //CS DIR setting
	LPC_GPIO->SET[0] = (1 << 6); //CS High
	LPC_SSP1->CR0 = 0x0707;
	LPC_SSP1->CPSR = 0x2;

	uint32_t i;
	for (i = 0; i < 8; i++) {
		dummy = LPC_SSP1->DR; /* clear the RxFIFO */
	}
	LPC_SSP1->CR1 = 1 << 1;
}

//MPU9250のWHOAMIを読むプログラム
int main(void) {
	SSP_Initialize();
	MPU9250_CS_ON;
	uint8_t data[100] = { };
	data[0] = 0xF5;
	SSP_Send((uint8_t *) data, 1);
	SSP_Receive((uint8_t *) data, 1);
	printf("WHOAMI = 0x%X\n", data[0]);
	MPU9250_CS_OFF;
	while (1) {
	}
	return 0;
}
