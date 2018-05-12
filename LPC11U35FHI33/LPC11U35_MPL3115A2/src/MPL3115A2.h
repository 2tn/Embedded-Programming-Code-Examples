/*
 * MPL3115A2.h
 *
 *  Created on: 2016/09/04
 *      Author: Yusuke
 */

#ifndef MPL3115A2_H_
#define MPL3115A2_H_

#include "LPC11Uxx.h"
#define MPL3115A2_ADDR	0x60
#define DR_STATUS		0x00
#define OUT_P_MSB		0x01
#define INT_SOURCE		0x12
#define PT_DATA_CFG		0x13
#define CTRL_REG1		0x26
#define CTRL_REG2		0x27
#define CTRL_REG3		0x28
#define CTRL_REG4		0x29
#define CTRL_REG5		0x2A

void MPL3115A2_WriteReg(uint8_t address, uint8_t data);
void MPL3115A2_ReadRegs(uint8_t address, uint8_t *data, uint8_t length);
void MPL3115A2_initialize(void);
void MPL3115A2_Read(uint32_t *pressure, uint16_t *temperture);

#endif /* MPL3115A2_H_ */
