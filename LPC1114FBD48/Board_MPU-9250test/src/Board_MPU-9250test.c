/*
 ===============================================================================
 Name        : Board_MPU-9250.c.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11xx.h"

#endif
#define SSP_BUFSIZE     16
#define SSPSR_TFE       (0x1<<0)
#define SSPSR_TNF       (0x1<<1)
#define SSPSR_RNE       (0x1<<2)
#define SSPSR_RFF       (0x1<<3)
#define SSPSR_BSY       (0x1<<4)

#define READ_FLAG         	0x80
#define WRITE_FLAG        	0x00

#define SMPLRT_DIV			0x19
#define CONFIG				0x1A
#define GYRO_CONFIG			0x1B
#define ACCEL_CONFIG      	0x1C
#define ACCEL_CONFIG2      	0x1D
#define I2C_MST_CTRL      	0x24
#define I2C_SLV0_ADDR     	0x25
#define I2C_SLV0_REG	   	0x26
#define I2C_SLV0_CTRL	   	0x27
#define I2C_SLV4_ADDR     	0x31
#define I2C_SLV4_REG      	0x32
#define I2C_SLV4_DO       	0x33
#define I2C_SLV4_CTRL     	0x34
#define I2C_SLV4_DI       	0x35
#define ACCEL_XOUT_H      	0x3B
#define ACCEL_ZOUT_H      	0x3F
#define ACCEL_ZOUT_L      	0x40
#define TEMP_OUT_H        	0x41
#define TEMP_OUT_L        	0x42
#define GYRO_ZOUT_H       	0x47
#define GYRO_ZOUT_L       	0x48
#define EXT_SENS_DATA_00  	0x49
#define I2C_SLV0_DO       	0x63
#define USER_CTRL         	0x6A
#define PWR_MGMT_1        	0x6B
#define PWR_MGMT_2        	0x6C
#define WHO_AM_I          	0x75

#define	CS_LOW()	{LPC_GPIO2->DATA &= ~(1<<10);}
#define	CS_HIGH()	{LPC_GPIO2->DATA |= (1<<10);}

void SPI0_Init();
void SPI0_Send(uint8_t *buf, uint32_t Length);
void SPI0_Receive(uint8_t *buf, uint32_t Length);
void SPI0_Read(uint8_t address, uint8_t length, uint8_t *data);
void SPI0_Write(uint8_t address, uint8_t data);

uint8_t src_addr[SSP_BUFSIZE];
uint8_t dest_addr[SSP_BUFSIZE];

int states = 0;

int main(void) {

	// Force the counter to be placed into memory
	volatile static int i = 0;
	// Enter an infinite loop, just incrementing a counter

	//initialization
	SPI0_Init();

	//initialization
	CS_HIGH()
	LPC_GPIO2->DIR |= (1 << 10);

	uint8_t ret;
	//CS Select
	CS_LOW()
	SPI0_Write(PWR_MGMT_1, 0x80);
	CS_HIGH()
	CS_LOW()
	SPI0_Write(ACCEL_CONFIG, 0x18);
	CS_HIGH()
	CS_LOW()
	SPI0_Write(GYRO_CONFIG, 0x00);
	CS_HIGH()

	CS_LOW()
	uint8_t res[1];
	SPI0_Read(ACCEL_CONFIG, 1, &res);
	CS_HIGH()
	while (1) {
		int acceldata[3];
		int gyrodata[3];
		uint8_t buf[6];
		CS_LOW()
		SPI0_Read(ACCEL_XOUT_H, 6, &buf[0]);
		CS_HIGH()
		int i;
		for (i = 0; i < 3; i++) {
			int tmp = buf[i * 2] << 8 | buf[i * 2 + 1];
			if (tmp >= 32768) {
				acceldata[i] = tmp - 65536;
			} else {
				acceldata[i] = tmp;
			}

			tmp = buf[i * 2 + 6] << 8 | buf[i * 2 + 7];
			if (tmp >= 32768) {
				gyrodata[i] = tmp - 65536;
			} else {
				gyrodata[i] = tmp;
			}
		}
		printf("%d\n", acceldata[0]);
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

	LPC_SSP0->CPSR = 0x00;
	LPC_SSP0->CR0 = (0x0707);
	/* SSPCPSR clock prescale register, master mode, minimum divisor is 0x02 */
	LPC_SSP0->CR1 = (1 << 1);
}

void SPI0_Read(uint8_t address, uint8_t length, uint8_t *data) {
	src_addr[0] = address | READ_FLAG;
	SPI0_Send((uint8_t *) src_addr, 1);
	SPI0_Receive(data, length);
}

void SPI0_Write(uint8_t address, uint8_t data) {
	src_addr[0] = address | WRITE_FLAG;
	SPI0_Send((uint8_t *) src_addr, 1);
	src_addr[0] = data;
	SPI0_Send((uint8_t *) src_addr, 1);
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
