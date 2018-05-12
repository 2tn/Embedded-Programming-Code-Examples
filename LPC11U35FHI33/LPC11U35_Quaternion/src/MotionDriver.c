/*
 * MotionDriver.c
 *
 *  Created on: 2016/06/10
 *      Author: Yusuke
 */

#ifndef MOTIONDRIVER_C_
#define MOTIONDRIVER_C_

#include "MotionDriver.h"
#include "timer.h"

/* Handle sensor on/off combinations. */
static void setup_gyro(void) {
	unsigned char mask = 0;
	if (hal.sensors & ACCEL_ON)
		mask |= INV_XYZ_ACCEL;
	if (hal.sensors & GYRO_ON)
		mask |= INV_XYZ_GYRO;
	/* If you need a power transition, this function should be called with a
	 * mask of the sensors still enabled. The driver turns off any sensors
	 * excluded from this mask.
	 */
	mpu_set_sensors(mask);
	if (!hal.dmp_on)
		mpu_configure_fifo(mask);
}

static void tap_cb(unsigned char direction, unsigned char count) {
	char data[2];
	data[0] = (char) direction;
	data[1] = (char) count;
}

static void android_orient_cb(unsigned char orientation) {
}

static inline void run_self_test(void) {
	int result;
	char test_packet[4] = { 0 };
	long gyro[3], accel[3];
	unsigned char i = 0;

#if defined (MPU6500) || defined (MPU9250)
	result = mpu_run_6500_self_test(gyro, accel, 0);
#elif defined (MPU6050) || defined (MPU9150)
	result = mpu_run_self_test(gyro, accel);
#endif
	if (result == 0x7) {
		/* Test passed. We can trust the gyro data here, so let's push it down
		 * to the DMP.
		 */
		for (i = 0; i < 3; i++) {
			gyro[i] = (long) (gyro[i] * 32.8f); //convert to +-1000dps
			accel[i] *= 2048.f; //convert to +-16G
			accel[i] = accel[i] >> 16;
			gyro[i] = (long) (gyro[i] >> 16);
		}

		mpu_set_gyro_bias_reg(gyro);

#if defined (MPU6500) || defined (MPU9250)
		mpu_set_accel_bias_6500_reg(accel);
#elif defined (MPU6050) || defined (MPU9150)
		mpu_set_accel_bias_6050_reg(accel);
#endif
	}

	/* Report results. */
	test_packet[0] = 't';
	test_packet[1] = result;
}

/* Every time new gyro data is available, this function is called in an
 * ISR context. In this example, it sets a flag protecting the FIFO read
 * function.
 */
static void gyro_data_ready_cb(void) {
	hal.new_gyro = 1;
}

/* Set up MSP430 peripherals. */
static inline void platform_init(void) {
	SysTick_initialize();
	I2C_Initialize();
}

void dmp_initialize(uint16_t frequency) {
	int result;
	unsigned char accel_fsr;
	unsigned short gyro_rate, gyro_fsr;
	unsigned long timestamp;
	struct int_param_s int_param;

	/* Set up MCU hardware. */
	platform_init();

	/* Set up gyro.
	 * Every function preceded by mpu_ is a driver function and can be found
	 * in inv_mpu.h.
	 */

	result = mpu_init(&int_param);

	/* If you're not using an MPU9150 AND you're not using DMP features, this
	 * function will place all slaves on the primary bus.
	 * mpu_set_bypass(1);
	 */
	/* Get/set hardware configuration. Start gyro. */
	/* Wake up all sensors. */
	mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	/* Push both gyro and accel data into the FIFO. */
	mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL);
	mpu_set_sample_rate(DEFAULT_MPU_HZ);
	/* Read back configuration in case it was set improperly. */
	mpu_get_sample_rate(&gyro_rate);
	mpu_get_gyro_fsr(&gyro_fsr);
	mpu_get_accel_fsr(&accel_fsr);

	/* Initialize HAL state variables. */
	memset(&hal, 0, sizeof(hal));
	hal.sensors = ACCEL_ON | GYRO_ON;

	/* To initialize the DMP:
	 * 1. Call dmp_load_motion_driver_firmware(). This pushes the DMP image in
	 *    inv_mpu_dmp_motion_driver.h into the MPU memory.
	 * 2. Push the gyro and accel orientation matrix to the DMP.
	 * 3. Register gesture callbacks. Don't worry, these callbacks won't be
	 *    executed unless the corresponding feature is enabled.
	 * 4. Call dmp_enable_feature(mask) to enable different features.
	 * 5. Call dmp_set_fifo_rate(freq) to select a DMP output rate.
	 * 6. Call any feature-specific control functions.
	 *
	 * To enable the DMP, just call mpu_set_dmp_state(1). This function can
	 * be called repeatedly to enable and disable the DMP at runtime.
	 *
	 * The following is a short summary of the features supported in the DMP
	 * image provided in inv_mpu_dmp_motion_driver.c:
	 * DMP_FEATURE_LP_QUAT: Generate a gyro-only quaternion on the DMP at
	 * 200Hz. Integrating the gyro data at higher rates reduces numerical
	 * errors (compared to integration on the MCU at a lower sampling rate).
	 * DMP_FEATURE_6X_LP_QUAT: Generate a gyro/accel quaternion on the DMP at
	 * 200Hz. Cannot be used in combination with DMP_FEATURE_LP_QUAT.
	 * DMP_FEATURE_TAP: Detect taps along the X, Y, and Z axes.
	 * DMP_FEATURE_ANDROID_ORIENT: Google's screen rotation algorithm. Triggers
	 * an event at the four orientations where the screen should rotate.
	 * DMP_FEATURE_GYRO_CAL: Calibrates the gyro data after eight seconds of
	 * no motion.
	 * DMP_FEATURE_SEND_RAW_ACCEL: Add raw accelerometer data to the FIFO.
	 * DMP_FEATURE_SEND_RAW_GYRO: Add raw gyro data to the FIFO.
	 * DMP_FEATURE_SEND_CAL_GYRO: Add calibrated gyro data to the FIFO. Cannot
	 * be used in combination with DMP_FEATURE_SEND_RAW_GYRO.
	 */
	dmp_load_motion_driver_firmware();
	dmp_set_orientation(inv_orientation_matrix_to_scalar(gyro_orientation));
	dmp_register_tap_cb(tap_cb);
	dmp_register_android_orient_cb(android_orient_cb);
	/*
	 * Known Bug -
	 * DMP when enabled will sample sensor data at 200Hz and output to FIFO at the rate
	 * specified in the dmp_set_fifo_rate API. The DMP will then sent an interrupt once
	 * a sample has been put into the FIFO. Therefore if the dmp_set_fifo_rate is at 25Hz
	 * there will be a 25Hz interrupt from the MPU device.
	 *
	 * There is a known issue in which if you do not enable DMP_FEATURE_TAP
	 * then the interrupts will be at 200Hz even if fifo rate
	 * is set at a different rate. To avoid this issue include the DMP_FEATURE_TAP
	 */
	hal.dmp_features = DMP_FEATURE_6X_LP_QUAT | DMP_FEATURE_TAP
			| DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL
			| DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
	dmp_enable_feature(hal.dmp_features);
	dmp_set_interrupt_mode(DMP_INT_CONTINUOUS);
	dmp_set_fifo_rate(frequency);
	mpu_set_dmp_state(1);
	hal.dmp_on = 1;

	unsigned long sensor_timestamp;
	short gyro[3], accel[3], sensors;
	unsigned char more;
	long quat[4], euler[3];

	long first_quat;
	dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
	first_quat = quat[0];
	while (gyro[0] == first_quat)
		dmp_read_fifo(gyro, accel, quat, &sensor_timestamp, &sensors, &more);
	Timer32_0_enable();
}

long inv_q29_mult(long a, long b) {
	long long temp;
	long result;
	temp = (long long) a * b;
	result = (long) (temp >> 29);
	return result;
}

/**
 *  @brief      Body-to-world frame euler angles.
 *  The euler angles are output with the following convention:
 *  Pitch: -180 to 180
 *  Roll: -90 to 90
 *  Yaw: -180 to 180
 *  @param[out] data        Euler angles in degrees, q16 fixed point.
 *  @param[out] accuracy    Accuracy of the measurement from 0 (least accurate)
 *                          to 3 (most accurate).
 *  @param[out] timestamp   The time in milliseconds when this sensor was read.
 *  @return     1 if data was updated.
 */
void quat2euler(long * quat, long *data) {
	long t1, t2, t3;
	long q00, q01, q02, q03, q11, q12, q13, q22, q23, q33;
	float values[3];

	q00 = inv_q29_mult(quat[0], quat[0]);
	q01 = inv_q29_mult(quat[0], quat[1]);
	q02 = inv_q29_mult(quat[0], quat[2]);
	q03 = inv_q29_mult(quat[0], quat[3]);
	q11 = inv_q29_mult(quat[1], quat[1]);
	q12 = inv_q29_mult(quat[1], quat[2]);
	q13 = inv_q29_mult(quat[1], quat[3]);
	q22 = inv_q29_mult(quat[2], quat[2]);
	q23 = inv_q29_mult(quat[2], quat[3]);
	q33 = inv_q29_mult(quat[3], quat[3]);

	/* X component of the Ybody axis in World frame */
	t1 = q12 - q03;

	/* Y component of the Ybody axis in World frame */
	t2 = q22 + q00 - (1L << 30);
	values[2] = -atan2f((float) t1, (float) t2) * 180.f / (float) M_PI;

	/* Z component of the Ybody axis in World frame */
	t3 = q23 + q01;
	values[0] = atan2f((float) t3, sqrtf((float) t1 * t1 + (float) t2 * t2))
			* 180.f / (float) M_PI;
	/* Z component of the Zbody axis in World frame */
	t2 = q33 + q00 - (1L << 30);
	if (t2 < 0) {
		if (values[0] >= 0)
			values[0] = 180.f - values[0];
		else
			values[0] = -180.f - values[0];
	}

	/* X component of the Xbody axis in World frame */
	t1 = q11 + q00 - (1L << 30);
	/* Y component of the Xbody axis in World frame */
	t2 = q12 + q03;
	/* Z component of the Xbody axis in World frame */
	t3 = q13 - q02;

	values[1] = (atan2f((float) (q33 + q00 - (1L << 30)), (float) (q13 - q02))
			* 180.f / (float) M_PI - 90);
	if (values[1] >= 90)
		values[1] = 180 - values[1];

	if (values[1] < -90)
		values[1] = -180 - values[1];
	data[0] = (long) (values[0] * 65536.f);
	data[1] = (long) (values[1] * 65536.f);
	data[2] = (long) (values[2] * 65536.f);
}

#endif /* MOTIONDRIVER_C_ */
