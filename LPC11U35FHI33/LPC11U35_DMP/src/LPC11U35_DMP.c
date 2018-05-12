/*
 ===============================================================================
 Name        : LPC11U35_DMP.c
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

#include "core\driver\eMPL\inv_mpu.h"
#include "core\driver\eMPL\inv_mpu_dmp_motion_driver.h"
#include "core\mllite\invensense.h"
#include "core\mpl\invensense_adv.h"
#include "core\eMPL-hal\eMPL_outputs.h"
#include "core\driver\include\mltypes.h"
#include "core\driver\include\mpu.h"
#include "core\driver\include\log.h"
#include "core\driver\LPC11U35\packet.h"

/**
 *   @defgroup  eMPL
 *   @brief     Embedded Motion Processing Library
 *
 *   @{
 *       @file      main.c
 *       @brief     Test app for eMPL using the Motion Driver DMP image.
 */

#define PRINT_ACCEL     (0x01)
#define PRINT_GYRO      (0x02)
#define PRINT_QUAT      (0x04)
#define PRINT_COMPASS   (0x08)
#define PRINT_EULER     (0x10)
#define PRINT_ROT_MAT   (0x20)
#define PRINT_HEADING   (0x40)
#define PRINT_PEDO      (0x80)
#define PRINT_LINEAR_ACCEL (0x100)
#define PRINT_GRAVITY_VECTOR (0x200)

volatile uint32_t hal_timestamp = 0;
#define ACCEL_ON        (0x01)
#define GYRO_ON         (0x02)
#define COMPASS_ON      (0x04)

#define MOTION          (0)
#define NO_MOTION       (1)

/* Starting sampling rate. */
#define DEFAULT_MPU_HZ  (20)

#define FLASH_SIZE      (512)
#define FLASH_MEM_START ((void*)0x1800)

#define PEDO_READ_MS    (1000)
#define TEMP_READ_MS    (500)
#define COMPASS_READ_MS (100)
struct rx_s {
	unsigned char header[3];
	unsigned char cmd;
};
struct hal_s {
	unsigned char lp_accel_mode;
	unsigned char sensors;
	unsigned char dmp_on;
	unsigned char wait_for_tap;
	volatile unsigned char new_gyro;
	unsigned char motion_int_mode;
	unsigned long no_dmp_hz;
	unsigned long next_pedo_ms;
	unsigned long next_temp_ms;
	unsigned long next_compass_ms;
	unsigned int report;
	unsigned short dmp_features;
	struct rx_s rx;
};
static struct hal_s hal = { 0 };

/* USB RX binary semaphore. Actually, it's just a flag. Not included in struct
 * because it's declared extern elsewhere.
 */
volatile unsigned char rx_new;

unsigned char *mpl_key = (unsigned char*) "eMPL 5.1";

/* Platform-specific information. Kinda like a boardfile. */
struct platform_data_s {
	signed char orientation[9];
};

/* The sensors can be mounted onto the board in any orientation. The mounting
 * matrix seen below tells the MPL how to rotate the raw data from the
 * driver(s).
 * TODO: The following matrices refer to the configuration on internal test
 * boards at Invensense. If needed, please modify the matrices to match the
 * chip-to-body matrix for your particular set up.
 */
static struct platform_data_s gyro_pdata = {.orientation = {1, 0, 0, 0, 1, 0,
		0, 0, 1}};

#if defined MPU9150 || defined MPU9250
static struct platform_data_s compass_pdata = {.orientation = {0, 1, 0, 1, 0,
		0, 0, 0, -1}};
#define COMPASS_ENABLED 1
#elif defined AK8975_SECONDARY
static struct platform_data_s compass_pdata = {
	.orientation = {-1, 0, 0,
		0, 1, 0,
		0, 0,-1}
};
#define COMPASS_ENABLED 1
#elif defined AK8963_SECONDARY
static struct platform_data_s compass_pdata = {
	.orientation = {-1, 0, 0,
		0,-1, 0,
		0, 0, 1}
};
#define COMPASS_ENABLED 1
#endif

/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* ---------------------------------------------------------------------------*/
/* Get data from MPL.
 * TODO: Add return values to the inv_get_sensor_type_xxx APIs to differentiate
 * between new and stale data.
 */
#ifdef COMPASS_ENABLED
void send_status_compass() {
	long data[3] = {0};
	int8_t accuracy = {0};
	unsigned long timestamp;
	inv_get_compass_set(data, &accuracy, (inv_time_t*) &timestamp);
	MPL_LOGI("Compass: %7.4f %7.4f %7.4f ", data[0] / 65536.f,
			data[1] / 65536.f, data[2] / 65536.f);
	MPL_LOGI("Accuracy= %d\r\n", accuracy);

}
#endif

/* Handle sensor on/off combinations. */
static void setup_gyro(void) {
	unsigned char mask = 0, lp_accel_was_on = 0;
	if (hal.sensors & ACCEL_ON)
		mask |= INV_XYZ_ACCEL;
	if (hal.sensors & GYRO_ON) {
		mask |= INV_XYZ_GYRO;
		lp_accel_was_on |= hal.lp_accel_mode;
	}
#ifdef COMPASS_ENABLED
	if (hal.sensors & COMPASS_ON) {
		mask |= INV_XYZ_COMPASS;
		lp_accel_was_on |= hal.lp_accel_mode;
	}
#endif
	/* If you need a power transition, this function should be called with a
	 * mask of the sensors still enabled. The driver turns off any sensors
	 * excluded from this mask.
	 */
	mpu_set_sensors(mask);
	mpu_configure_fifo(mask);
	if (lp_accel_was_on) {
		unsigned short rate;
		hal.lp_accel_mode = 0;
		/* Switching out of LP accel, notify MPL of new accel sampling rate. */
		mpu_get_sample_rate(&rate);
		inv_set_accel_sample_rate(1000000L / rate);
	}
}
void gyro_data_ready_cb(void) {
	hal.new_gyro = 1;
}

void quat2eular(long *quat, long *data) {
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

	return 0;
}

/*******************************************************************************/

/**
 * @brief main entry point.
 * @par Parameters None
 * @retval void None
 * @par Required preconditions: None
 */

int main(void) {

	inv_error_t result;
	unsigned char accel_fsr, new_temp = 0;
	unsigned short gyro_rate, gyro_fsr;
	unsigned long timestamp;
	struct int_param_s int_param;

#ifdef COMPASS_ENABLED
	unsigned char new_compass = 0;
	unsigned short compass_fsr;
#endif

	result = mpu_init(&int_param);

	/* If you're not using an MPU9150 AND you're not using DMP features, this
	 * function will place all slaves on the primary bus.
	 * mpu_set_bypass(1);
	 */

	result = inv_init_mpl();
	result = inv_enable_quaternion();
	result = inv_enable_9x_sensor_fusion();
	result = inv_enable_gyro_tc();
	inv_enable_eMPL_outputs();
	result = inv_start_mpl();
	if (result == INV_ERROR_NOT_AUTHORIZED) {
		while (1) {
			MPL_LOGE("Not authorized.\n");
		}
	}
	if (result) {
		MPL_LOGE("Could not start the MPL.\n");
	}
	/*
	 mpu_set_sensors(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
	 mpu_configure_fifo(INV_XYZ_GYRO | INV_XYZ_ACCEL | INV_XYZ_COMPASS);
	 mpu_set_sample_rate(DEFAULT_MPU_HZ);
	 mpu_set_compass_sample_rate(1000 / COMPASS_READ_MS);
	 mpu_get_sample_rate(&gyro_rate);
	 mpu_get_gyro_fsr(&gyro_fsr);
	 mpu_get_accel_fsr(&accel_fsr);
	 mpu_get_compass_fsr (&compass_fsr);

	 hal.sensors = ACCEL_ON | GYRO_ON | COMPASS_ON;
	 hal.dmp_on = 0;
	 hal.report = 0;
	 hal.rx.cmd = 0;
	 hal.next_pedo_ms = 0;
	 hal.next_compass_ms = 0;
	 hal.next_temp_ms = 0;

	 /* Compass reads are handled by scheduler. */
//	get_ms(&timestamp);
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
	dmp_set_orientation(
			inv_orientation_matrix_to_scalar(gyro_pdata.orientation));
	hal.dmp_features = DMP_FEATURE_LP_QUAT | DMP_FEATURE_TAP

	| DMP_FEATURE_ANDROID_ORIENT | DMP_FEATURE_SEND_RAW_ACCEL
			| DMP_FEATURE_SEND_CAL_GYRO | DMP_FEATURE_GYRO_CAL;
	dmp_enable_feature(hal.dmp_features);
	dmp_set_fifo_rate(DEFAULT_MPU_HZ);
	mpu_set_dmp_state(1);
	hal.dmp_on = 1;

	inv_enable_eMPL_outputs();
	while (1) {
		short gyro[3];
		short accel[3];
		short sensor[3];
		long quat[4];
		char more[2];
		long eular[2];
		dmp_read_fifo(&gyro, &accel, &quat, &timestamp, &sensor, &more);
		quat2eular((long *) quat, (long *) eular);

		printf("%ld,%ld,%ld,%ld,%.2f,%.2f,%.2f\n", quat[0], quat[1], quat[2],
				quat[3], (float) eular[0] / 65536.f, (float) eular[1] / 65536.f,
				(float) eular[2] / 65536.f);
		long data[4];
		int8_t accuracy[0];
		inv_time_t timestamp[0];

		inv_get_sensor_type_quat((long *) data, (int8_t *) accuracy,
				(inv_time_t *) timestamp);

	}
}
