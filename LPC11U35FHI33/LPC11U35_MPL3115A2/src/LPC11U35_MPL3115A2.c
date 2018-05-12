/*
 ===============================================================================
 Name        : LPC11U35_MPL3115A2.c
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
#include "i2c.h"
#include "MPL3115A2.h"
#include "interrupt.h"
#include <stdio.h>

uint8_t flag = 0;
uint8_t data[6];

int main(void) {
	interrupt_initialize();
	I2C_Initialize();
	MPL3115A2_initialize();

	while (1) {
		if (flag) {
			uint32_t pressure;
			uint16_t temperture;
			MPL3115A2_Read(&pressure, &temperture);
			printf("%.2f %.2f\n", (double) pressure / 400,
					(double) temperture / 16);
			flag = 0;
		}
	}
	return 0;
}
