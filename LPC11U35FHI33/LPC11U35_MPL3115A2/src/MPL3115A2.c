/*
 * MPL3115A2.c
 *
 *  Created on: 2016/09/04
 *      Author: Yusuke
 */

#include "MPL3115A2.h"
#include "i2c.h"
#include "LPC11Uxx.h"

void MPL3115A2_WriteReg(uint8_t address, uint8_t data) {
	uint8_t tmp[1] = { data };
	I2C_Write(MPL3115A2_ADDR, address, tmp, 1);
}

void MPL3115A2_ReadRegs(uint8_t address, uint8_t *data, uint8_t length) {
	I2C_Read(MPL3115A2_ADDR, address, data, length);
}

void MPL3115A2_initialize(void) {
	uint8_t data[6];
	MPL3115A2_WriteReg(CTRL_REG1, ((1 << 0) | (0x7 << 3)));
	MPL3115A2_WriteReg(PT_DATA_CFG, 0x7);
	MPL3115A2_WriteReg(CTRL_REG2, 0);
	MPL3115A2_WriteReg(CTRL_REG3, 1 << 5);
	MPL3115A2_WriteReg(CTRL_REG4, 1 << 7);
	MPL3115A2_WriteReg(CTRL_REG5, 1 << 7);
	MPL3115A2_ReadRegs(DR_STATUS, data, 6);
}

void MPL3115A2_Read(uint32_t *pressure, uint16_t *temperture) {
	uint8_t data[6];
	MPL3115A2_ReadRegs(DR_STATUS, data, 6);
	*pressure = data[1] << 12 | data[2] << 4 | data[3] >> 4;
	*temperture = data[4] << 4 | data[5] >> 4;
}
