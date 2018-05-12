/*
 ===============================================================================
 Name        : LPC11U35_GPS.c
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
#include "uart.h"
#include "i2c.h"
#include "gps.h"
// TODO: insert other include files here

// TODO: insert other definitions and declarations here

static volatile uint8_t flag = 0;
uint64_t time = 0;
void SysTick_Handler(void) {
	time++;
}
void UART_IRQHandler() {
	uint16_t IIRValue;
	uint8_t regVal;
	IIRValue = LPC_USART->IIR;
	IIRValue >>= 1;
	IIRValue &= 0x07;
	if (IIRValue == 0x02) {
		regVal = LPC_USART->RBR;
	}
}

void TIMER32_1_IRQHandler(void) {
	if ( LPC_CT32B1->IR & (0x01 << 0)) {
		LPC_CT32B1->IR = 0x1 << 0;
		LPC_CT32B1->TCR = 2;
		flag = 1;
	}
}

void FLEX_INT0_IRQHandler() {
	if ( LPC_GPIO_PIN_INT->IST & (0x1 << 0)) {
		if ( LPC_GPIO_PIN_INT->ISEL & (0x1 << 0)) {
		} else {
			if (( LPC_GPIO_PIN_INT->RISE & (0x1 << 0))
					&& ( LPC_GPIO_PIN_INT->IENR & (0x1 << 0))) {
				LPC_GPIO_PIN_INT->RISE = 0x1 << 0;
				flag = 2;
			}
			LPC_GPIO_PIN_INT->IST = 0x1 << 0;
		}
	}
	return;
}

int main(void) {
	int i;
	UART_Initialize(115200);
	UART_printf("Hello, UART!\r\n");
	GPS_initialize();
	while (1) {
		if (flag) {
			uint16_t length = GPS_fifo_length();
			switch (flag) {
			case 1: {
				uint8_t data[length];
				if (GPS_read_fifo(data, length) == 0xB5620107) {
					NAV_PVT pvt;
					GPS_read_NAV_PVT(data,&pvt);
					printf("%d/%02d/%02d %02d:%02d:%02d\r\n", pvt.year,
							pvt.month,  pvt.day, pvt.hour, pvt.min, pvt.sec);
					printf(
							"Latitude:%lld Longitude:%lld GroundSpeed:%lld Heading:%lld\r\n",
							 pvt.lat, pvt.lon, pvt.gSpeed, pvt.heading);
					printf("valid:0x%02x fixType:0x%02x flags:0x%02x\r\n",
							 pvt.valid, pvt.fixType, pvt.flags);
				} else {
					printf("error len:%d\r\n", length);
				}
				break;
			}
			case 2: {
				Timer32_1_start();
				break;
			}
			default:
				break;
			}
			flag = 0;
		}
	}
	return 0;
}
