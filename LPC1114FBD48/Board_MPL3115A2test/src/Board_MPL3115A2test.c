/*
 ===============================================================================
 Name        : Board_I2Ctest.c

 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */
#include "LPC11xx.h"			/* LPC11xx Peripheral Registers */
#include "i2c.h"

extern volatile uint32_t I2CCount;
extern volatile uint8_t I2CMasterBuffer[BUFSIZE];
extern volatile uint8_t I2CSlaveBuffer[BUFSIZE];
extern volatile uint32_t I2CMasterState;
extern volatile uint32_t I2CReadLength, I2CWriteLength;

void I2C_Write(uint8_t address, uint8_t data);
void I2C_Read(uint8_t address, uint8_t length);

void printf();
int interrupt = 0;

void PIOINT1_IRQHandler(void) {
	interrupt = 1;
	LPC_GPIO1->IC |= (1 << 7);
}
int main(void) {
	int i;
	SystemCoreClockUpdate();

	if (I2CInit((uint32_t) I2CMASTER) == 0) /* initialize I2c */
	{
		while (1)
			; /* Fatal error */
	}

	NVIC_EnableIRQ (EINT1_IRQn);
	LPC_GPIO1->DIR &= ~(1 << 7);
	LPC_GPIO1->IBE &= ~(1 << 7);
	LPC_GPIO1->IEV |= (1 << 7);
	LPC_GPIO1->IE |= (1 << 7);

	I2C_Write(CTRL_REG1, 0x00);
	I2C_Write(PT_DATA_CFG, 0x07);
	I2C_Write(CTRL_REG2, 0x00);
	I2C_Write(CTRL_REG3, (1 << 5));
	I2C_Write(CTRL_REG4, (1 << 7));
	I2C_Write(CTRL_REG5, (1 << 7));
	I2C_Write(CTRL_REG1, 0x02);
	I2C_Read(OUT_P_MSB, 5);

	while (1) {
		if (1) {
			I2C_Write(CTRL_REG1, 0x02);
			I2C_Read(OUT_P_MSB, 5);
			printf("%f %f\n",
					((double) (I2CSlaveBuffer[0] << 12 | I2CSlaveBuffer[1] << 4
							| I2CSlaveBuffer[2] >> 4)) / 400,
					((double) (I2CSlaveBuffer[3] << 4 | I2CSlaveBuffer[4] >> 4))
							/ 16);
			interrupt = 0;
		}

	}
}

void I2C_Write(uint8_t address, uint8_t data) {

	I2CWriteLength = 3;
	I2CReadLength = 0;
	I2CMasterBuffer[0] = MPL3115A2_ADDR << 1;
	I2CMasterBuffer[1] = address; /* address */
	I2CMasterBuffer[2] = data;
	I2CEngine();
}
void I2C_Read(uint8_t address, uint8_t length) {
	int i;
	for (i = 0; i < BUFSIZE; i++) {
		I2CSlaveBuffer[i] = 0x00;
	}
	/* Write SLA(W), address, SLA(R), and read one byte back. */
	I2CWriteLength = 2;
	I2CReadLength = length;
	I2CMasterBuffer[0] = MPL3115A2_ADDR << 1;
	I2CMasterBuffer[1] = address; /* address */
	I2CMasterBuffer[2] = (MPL3115A2_ADDR << 1) | RD_BIT;
	I2CEngine();
}
