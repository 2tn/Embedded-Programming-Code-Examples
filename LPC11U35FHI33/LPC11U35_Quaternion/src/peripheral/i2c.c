/*
 ===============================================================================
 Name        : LPC11U35_I2C.c
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
#include "i2c.h"

uint32_t I2CMasterState;
uint32_t RdIndex = 0;
uint32_t WrIndex = 0;
uint32_t timeout = 0;

volatile uint8_t I2CMasterBuffer[BUFSIZE];
volatile uint8_t *I2CSlaveBuffer;
volatile uint32_t I2CReadLength;
volatile uint32_t I2CWriteLength;

void I2C_IRQHandler(void) {
	uint8_t StatValue;

	timeout = 0;
	/* this handler deals with master read and master write only */
	StatValue = LPC_I2C->STAT;
	//printf("%x\n", StatValue);
	switch (StatValue) {
	case 0x08: /* A Start condition is issued. */
		WrIndex = 0;
		RdIndex = 0;
		LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		break;

	case 0x10: /* A repeated started is issued */
		RdIndex = 0;
		/* Send SLA with R bit set, */
		LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
		LPC_I2C->CONCLR = (I2CONCLR_SIC | I2CONCLR_STAC);
		break;

	case 0x18: /* Regardless, it's a ACK */
		if (I2CWriteLength == 1) {
			LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
			I2CMasterState = I2C_NO_DATA;
		} else {
			LPC_I2C->DAT = I2CMasterBuffer[WrIndex++];
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x28: /* Data byte has been transmitted, regardless ACK or NACK */
		if (WrIndex < I2CWriteLength) {
			LPC_I2C->DAT = I2CMasterBuffer[WrIndex++]; /* this should be the last one */
		} else {
			if (I2CReadLength != 0) {
				LPC_I2C->CONSET = I2CONSET_STA; /* Set Repeated-start flag */
			} else {
				LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
				I2CMasterState = I2C_OK;
			}
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x30:
		LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
		I2CMasterState = I2C_NACK_ON_DATA;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x40: /* Master Receive, SLA_R has been sent */
		if ((RdIndex + 1) < I2CReadLength) {
			/* Will go to State 0x50 */
			LPC_I2C->CONSET = I2CONSET_AA; /* assert ACK after data is received */
		} else {
			/* Will go to State 0x58 */
			LPC_I2C->CONCLR = I2CONCLR_AAC; /* assert NACK after data is received */
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x50: /* Data byte has been received, regardless following ACK or NACK */
		*I2CSlaveBuffer = LPC_I2C->DAT;
		I2CSlaveBuffer++;
		RdIndex++;
		if ((RdIndex + 1) < I2CReadLength) {
			LPC_I2C->CONSET = I2CONSET_AA; /* assert ACK after data is received */
		} else {
			LPC_I2C->CONCLR = I2CONCLR_AAC; /* assert NACK on last byte */
		}
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x58:
		*I2CSlaveBuffer = LPC_I2C->DAT;
		I2CSlaveBuffer++;
		RdIndex++;
		I2CMasterState = I2C_OK;
		LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
		LPC_I2C->CONCLR = I2CONCLR_SIC; /* Clear SI flag */
		break;

	case 0x20: /* regardless, it's a NACK */
	case 0x48:
		LPC_I2C->CONSET = I2CONSET_STO; /* Set Stop flag */
		I2CMasterState = I2C_NACK_ON_ADDRESS;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;

	case 0x38: /* Arbitration lost, in this example, we don't
	 deal with multiple master situation */
	default:
		I2CMasterState = I2C_ARBITRATION_LOST;
		LPC_I2C->CONCLR = I2CONCLR_SIC;
		break;
	}
	return;
}

uint32_t I2C_Engine(void) {
	RdIndex = 0;
	WrIndex = 0;

	LPC_I2C->CONSET = I2CONSET_STA; /* Set Start flag */
	I2CMasterState = I2C_BUSY;
	while (I2CMasterState == I2C_BUSY) {
		if (timeout >= MAX_TIMEOUT) {
			I2CMasterState = I2C_TIME_OUT;
			break;
		}
		timeout++;
	}
	LPC_I2C->CONCLR = I2CONCLR_STAC;
	return (I2CMasterState);
}

void I2C_Initialize(void) {
	//IO settings
	LPC_IOCON->PIO0_4 = 0x1;
	LPC_IOCON->PIO0_5 = 0x1;
	LPC_SYSCON->SYSAHBCLKCTRL |= (1 << 5);
	LPC_SYSCON->PRESETCTRL |= (1 << 1);

	//reset settings and set a clock
	LPC_I2C->CONCLR = I2CONCLR_AAC | I2CONCLR_I2ENC | I2CONCLR_SIC
			| I2CONCLR_STAC;
	LPC_I2C->SCLH = 180;
	LPC_I2C->SCLL = 180;
	NVIC_EnableIRQ (I2C_IRQn);

	//enable i2c module
	LPC_I2C->CONSET = I2CONSET_I2EN;
}

int I2C_Read(unsigned char device, unsigned char address, unsigned char length,
		unsigned char *data) {
	I2CWriteLength = 2;
	I2CReadLength = length;
	uint32_t i;
	I2CSlaveBuffer = data;
	for (i = 0; i < BUFSIZE; i++) {
		I2CMasterBuffer[i] = 0;
	}
	while (length != 0) {
		*data = 0;
		length--;
	}
	I2CMasterBuffer[0] = (device << 1);
	I2CMasterBuffer[1] = address;
	I2CMasterBuffer[2] = (device << 1) | 0x1;
	if (I2C_Engine() == I2C_TIME_OUT) {
		return -1;
	}
	return 0;
}

int I2C_Write(unsigned char device, unsigned char address, unsigned char length,
		unsigned char const *data) {
	I2CWriteLength = length + 2;
	I2CReadLength = 0;
	uint32_t i;
	for (i = 0; i < BUFSIZE; i++) {
		I2CMasterBuffer[i] = 0;
	}
	I2CMasterBuffer[0] = (device << 1);
	I2CMasterBuffer[1] = address;
	for (i = 0; i < length; i++) {
		I2CMasterBuffer[i + 2] = *data;
		data++;
	}
	if (I2C_Engine() == I2C_TIME_OUT) {
		return -1;
	}
	return 0;
}

