/*
 * MotionDriver.h
 *
 *  Created on: 2016/06/10
 *      Author: Yusuke
 */

#ifndef MOTIONDRIVER_H_
#define MOTIONDRIVER_H_

#include <math.h>
#include "inv_mpu.h"
#include "inv_mpu_dmp_motion_driver.h"
#include "LPC11Uxx.h"

/* Data requested by client. */
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define DMP_OFF		(0x00)
#define DMP_ON		(0x01)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (100)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)
#ifndef M_PI
#define M_PI 3.14159265358979
#endif


struct hal_s {
	unsigned char sensors;
	unsigned char dmp_on;
	unsigned char wait_for_tap;
	volatile unsigned char new_gyro;
	unsigned short dmp_features;
	unsigned char motion_int_mode;
};
static struct hal_s hal = { 0 };

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from thei
 * driver(s).
 * TODO: The following matrices refer to the configuration on an internal test
 * board at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static signed char gyro_orientation[9] = { -1, 0, 0, 0, -1, 0, 0, 0, 1 };

/* These next two functions converts the orientation matrix (see
 * gyro_orientation) to a scalar representation for use by the DMP.
 * NOTE: These functions are borrowed from Invensense's MPL.
 */
static inline unsigned short inv_row_2_scale(const signed char *row) {
	unsigned short b;

	if (row[0] > 0)
		b = 0;
	else if (row[0] < 0)
		b = 4;
	else if (row[1] > 0)
		b = 1;
	else if (row[1] < 0)
		b = 5;
	else if (row[2] > 0)
		b = 2;
	else if (row[2] < 0)
		b = 6;
	else
		b = 7;      // error
	return b;
}

static inline unsigned short inv_orientation_matrix_to_scalar(
		const signed char *mtx) {
	unsigned short scalar;

	/*
	 XYZ  010_001_000 Identity Matrix
	 XZY  001_010_000
	 YXZ  010_000_001
	 YZX  000_010_001
	 ZXY  001_000_010
	 ZYX  000_001_010
	 */

	scalar = inv_row_2_scale(mtx);
	scalar |= inv_row_2_scale(mtx + 3) << 3;
	scalar |= inv_row_2_scale(mtx + 6) << 6;

	return scalar;
}


void dmp_initialize(uint16_t frequency);

void quat2euler(long * quat, long *data) ;

#endif /* MOTIONDRIVER_H_ */
