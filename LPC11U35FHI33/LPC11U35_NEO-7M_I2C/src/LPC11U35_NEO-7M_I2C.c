/*
 ===============================================================================
 Name        : LPC11U35_NEO-7M_I2C.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11Uxx.h"
#endif

#define MODE	0

#include <cr_section_macros.h>

#include <stdio.h>
#include "i2c.h"
#include "uart.h"
#include "interrupt.h"

uint8_t flag = 0;

void UBX_send(uint8_t class, uint8_t id, uint16_t length, uint8_t *data) {
	int i;
	uint8_t CK_A = 0, CK_B = 0;
	uint8_t out[length + 8];
	out[0] = 0xB5;
	out[1] = 0x62;
	out[2] = class;
	out[3] = id;
	out[4] = length;
	out[5] = length >> 8;

	for (i = 0; i < length + 4; i++) {
		if (i >= 4) {
			out[i + 2] = *data;
			data++;
		}
		CK_A = CK_A + out[i + 2];
		CK_B = CK_B + CK_A;
	}
	out[length + 6] = CK_A;
	out[length + 7] = CK_B;
	I2C_ublox_Write(out, length + 8);
}

void FLEX_INT0_IRQHandler(void) {
	if ( LPC_GPIO_PIN_INT->IST & (0x1 << 0)) {
		if ( LPC_GPIO_PIN_INT->ISEL & (0x1 << 0)) {
		} else {
			if (( LPC_GPIO_PIN_INT->RISE & (0x1 << 0))
					&& ( LPC_GPIO_PIN_INT->IENR & (0x1 << 0))) {
				LPC_GPIO_PIN_INT->RISE = 0x1 << 0;
				printf("TIMEPULSE\r\n", 11);
				flag = 5;
			}
			LPC_GPIO_PIN_INT->IST = 0x1 << 0;
		}
	}
	return;
}
void UART_IRQHandler(void) {
	uint16_t IIRValue;
	uint8_t regVal;
	IIRValue = LPC_USART->IIR;
	IIRValue >>= 1;
	IIRValue &= 0x07;
	if (IIRValue == 0x02) {
		regVal = LPC_USART->RBR;
		switch (regVal) {
		case 'd':
			printf("case:d\n");
			flag = 1;
			break;
		case 'e':
			printf("case:e\n");
			flag = 2;
			break;
		case 't':
			printf("case:t\n");
			flag = 3;
			break;
		case 'y':
			printf("case:y\n");
			flag = 4;
			break;
		default:
			break;
		}
	}
}

int main(void) {
	int i;
	I2C_Initialize();
	UART_Initialize(115200);
	interrupt_initialize();
	uint8_t data1[1000] = { 0 };
	while (1) {
		uint8_t data[1000];
		uint16_t length = 0;
		I2C_Read(0x42, 0xFD, (uint8_t *) data, 2);
		length = (data[0] << 8) | data[1];

		if (length) {
			if (length == 0xFFFF)
				continue;
			if (length > 1000)
				length = 1000;
			I2C_Read(0x42, 0xFF, (uint8_t *) data, length);
			uint64_t header = (data[0] << 24 | data[1] << 16 | data[2] << 8
					| data[3]) & 0xFFFFFFFF;
			if (header == 0xB5620501) {
				UART_Send("Acknowledged\r\n", 15);
			} else if (header == 0xB5620500) {
				UART_Send("Not-Acknowledged\r\n", 19);
			} else {
				//UART_Send(data, length);
				printf("%s\n",data);
			}
		}
		switch (flag) {
		case 1: {
			for (i = 0; i < 6; i++) {
				/*
				 * 	Please note that I/O Ports 1 and 2 correspond to serial ports 1 and 2.
				 *  I/O port 0 is DDC. I/O port 3 is USB. I/O port 4 is SPI.
				 *  I/O port 5 is reserved for future use.
				 */
				uint8_t d[8] = { 0xF0, i, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
				UBX_send(0x06, 0x01, 8, d);
			}
			break;
		}
		case 2: {
			for (i = 0; i < 6; i++) {
				uint8_t d[8] = { 0xF0, i, 0x01, 0x00, 0x00, 0x00, 0x00, 0x01 };
				UBX_send(0x06, 0x01, 8, d);
			}
			break;
		}
		case 3: {
			uint8_t d[32] = { 0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
					0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x99, 0x99,
					0x99, 0x19, 0x99, 0x99, 0x99, 0x19, 0x00, 0x00, 0x00, 0x00,
					0x6F, 0x00, 0x00, 0x00 };
			UBX_send(0x06, 0x31, 32, d);
			break;
		}
		case 4: {
			uint8_t d[32] = { 0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
					0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x99, 0x99,
					0x99, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
					0x00, 0x00, 0x00, 0x00 };
			UBX_send(0x06, 0x31, 32, d);
			break;
		}
		case 5: {
			uint8_t gll[15] = { 0x24, 0x45, 0x49, 0x47, 0x50, 0x51, 0x2C, 0x47,
					0x4C, 0x4C, 0x2A, 0x32, 0x31, 0x0D, 0x0A };
			I2C_ublox_Write(gll, 15);
			break;
		}
		default:
			break;
		}
		flag = 0;
	}
	return 0;
}
