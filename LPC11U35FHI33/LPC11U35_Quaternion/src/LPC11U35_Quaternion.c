/*
 ===============================================================================
 Name        : LPC11U35_Quaternion.c
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
#include "MotionDriver.h"
#include <stdio.h>
#include "i2c.h"

#define DMP_FREQUENCY	100

unsigned long sensor_timestamp;
short gyro[3], accel[3], sensors;
unsigned char more;
long quat[4], euler[3];

uint8_t quat_flag = 0;

void TIMER32_0_IRQHandler(void) {
	if ( LPC_CT32B0->IR & (0x01 << 0)) {
		LPC_CT32B0->IR = 0x1 << 0;
		quat_flag = 1;
	}
}

int main(void) {
	Timer32_0_initialize(DMP_FREQUENCY);
	dmp_initialize(DMP_FREQUENCY);
	while (1) {
		if (quat_flag) {
			dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors,
					&more);
			quat2euler(quat, euler);
			printf("%.2f,%.2f,%.2f\n", (float) euler[0] / 65536.f,
					(float) euler[1] / 65536.f, (float) euler[2] / 65536.f);
			quat_flag = 0;
		}
	}
}

