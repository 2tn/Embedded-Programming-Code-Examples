/*
 ===============================================================================
 Name        : Board_ADXL375test.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#define SSP_BUFSIZE     16
#define SSPSR_TFE       (0x1<<0)
#define SSPSR_TNF       (0x1<<1)
#define SSPSR_RNE       (0x1<<2)
#define SSPSR_RFF       (0x1<<3)
#define SSPSR_BSY       (0x1<<4)

#define READ_FLAG         0x80
#define WRITE_FLAG        0x00
#define WHO_AM_I          0x75
#define	CS_LOW()	{LPC_GPIO1->DATA &= ~(1<<2);}
#define	CS_HIGH()	{LPC_GPIO1->DATA |= (1<<2);}

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
void SPI_Init(uint8_t portnum);
void SPI_Send(uint8_t portNum, uint8_t *buf, uint32_t Length);
void SPI_Receive(uint8_t portNum, uint8_t *buf, uint32_t Length);
uint8_t SPI_Read(int address);
void SPI_Write(int address, uint8_t data);

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];

int main(void) {

	printf("Hello World\n");

	//initialization
	SPI0_Init();

	//initialization
	CS_HIGH()
	;
	LPC_GPIO1->DIR |= (1 << 2);

	uint8_t ret;
	//CS Select
	ret = SPI_Read(WHO_AM_I);
	// Force the counter to be placed into memory
	volatile static int i = 0;
	// Enter an infinite loop, just incrementing a counter
	while (1) {
		i++;
	}
	return 0;
}

void SPI0_Init() {

	LPC_SYSCON->PRESETCTRL |= (0x1 << 0);
	LPC_SYSCON->SYSAHBCLKCTRL |= (0x1 << 11);
	LPC_SYSCON->SSP0CLKDIV = 0x02; /* Divided by 2 */
	LPC_IOCON->PIO0_8 |= 0x01; /* SSP MISO */
	LPC_IOCON->PIO0_9 |= 0x01; /* SSP MOSI */
	LPC_IOCON->SCK_LOC = 0x02;
	LPC_IOCON->PIO0_6 = 0x02; /* P0.6 function 2 is SSP clock, need to*/

	LPC_SSP0->CPSR = 0x40;
	LPC_SSP0->CR0 = (0x07C7); //CPOL=1,CPHA=1
	/* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
	LPC_SSP0->CR1 = (1 << 1);
}

void SPI0_Send(uint8_t *buf, uint32_t Length) {
	uint32_t i;
	uint8_t Dummy = Dummy;

	for (i = 0; i < Length; i++) {
		while ((LPC_SSP0->SR & (SSPSR_TNF | SSPSR_BSY)) != SSPSR_TNF)
			;
		LPC_SSP0->DR = *buf;
		buf++;
		while ((LPC_SSP0->SR & (SSPSR_BSY | SSPSR_RNE)) != SSPSR_RNE)
			;
		Dummy = LPC_SSP0->DR;
	}
	return;
}
void SPI0_Receive(uint8_t *buf, uint32_t Length) {
	uint32_t i;
	for (i = 0; i < Length; i++) {
		LPC_SSP0->DR = 0xFF;
		while ((LPC_SSP0->SR & (SSPSR_BSY | SSPSR_RNE)) != SSPSR_RNE)
			;
		*buf = LPC_SSP0->DR;
		buf++;
	}
	return;
}

uint8_t SPI_Read(int address) {
	CS_LOW()
	src_addr[0] = address | READ_FLAG;
	SPI0_Send((uint8_t *) src_addr, 1);
	SPI0_Receive((uint8_t *) dest_addr, 1);
	CS_HIGH()
	return dest_addr[0];
}

void SPI_Write(int address, uint8_t data) {
	CS_LOW()
	src_addr[0] = address | WRITE_FLAG;
	SPI0_Send((uint8_t *) src_addr, 1);
	src_addr[0] = data;
	SPI0_Send((uint8_t *) src_addr, 1);
	CS_HIGH()
}
