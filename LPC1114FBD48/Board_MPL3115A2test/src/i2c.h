/****************************************************************************
 *   $Id:: i2c.h 3662 2010-06-03 19:47:02Z usb00423                         $
 *   Project: NXP LPC11xx I2C example
 *
 *   Description:
 *     This file contains I2C code header definition.
 *
 ****************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 ****************************************************************************/
#ifndef __I2C_H 
#define __I2C_H

/* If I2C SEEPROM is tested, make sure FAST_MODE_PLUS is 0.
 For board to board test, this flag can be turned on. */

#define FAST_MODE_PLUS      0

#define BUFSIZE             64
#define MAX_TIMEOUT         0x00FFFFFF

#define I2CMASTER           0x01
#define I2CSLAVE            0x02

//MPL3115A2 address
#define MPL3115A2_ADDR      0x60
#define I2C_ADDR         	0x60
#define STATUS           	0x00
#define OUT_P_MSB        	0x01
#define OUT_P_CSB        	0x02
#define OUT_P_LSB        	0x03
#define OUT_T_MSB        	0x04
#define OUT_T_LSB        	0x05
#define DR_STATUS        	0x06
#define OUT_P_DELTA_MSB  	0x07
#define OUT_P_DELTA_CSB  	0x08
#define OUT_P_DELTA_LSB  	0x09
#define OUT_T_DELTA_MSB  	0x0A
#define OUT_T_DELTA_LSB  	0x0B
#define WHO_AM_I         	0x0C
#define CTRL_REG1       	0x26
#define CTRL_REG2        	0x27
#define CTRL_REG3        	0x28
#define CTRL_REG4        	0x29
#define CTRL_REG5        	0x2A
#define PT_DATA_CFG      	0x13
#define INT_SOURCE			0x12
//end address

#define READ_WRITE          0x01

#define RD_BIT              0x01

#define I2C_IDLE              0
#define I2C_STARTED           1
#define I2C_RESTARTED         2
#define I2C_REPEATED_START    3
#define DATA_ACK              4
#define DATA_NACK             5
#define I2C_BUSY              6
#define I2C_NO_DATA           7
#define I2C_NACK_ON_ADDRESS   8
#define I2C_NACK_ON_DATA      9
#define I2C_ARBITRATION_LOST  10
#define I2C_TIME_OUT          11
#define I2C_OK                12

#define I2CONSET_I2EN       (0x1<<6)  /* I2C Control Set Register */
#define I2CONSET_AA         (0x1<<2)
#define I2CONSET_SI         (0x1<<3)
#define I2CONSET_STO        (0x1<<4)
#define I2CONSET_STA        (0x1<<5)

#define I2CONCLR_AAC        (0x1<<2)  /* I2C Control clear Register */
#define I2CONCLR_SIC        (0x1<<3)
#define I2CONCLR_STAC       (0x1<<5)
#define I2CONCLR_I2ENC      (0x1<<6)

#define I2DAT_I2C           0x00000000  /* I2C Data Reg */
#define I2ADR_I2C           0x00000000  /* I2C Slave Address Reg */
#define I2SCLH_SCLH         0x000001E0  /* I2C SCL Duty Cycle High Reg */
#define I2SCLL_SCLL         0x000001E0  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_HS_SCLH		0x00000015  /* Fast Plus I2C SCL Duty Cycle High Reg */
#define I2SCLL_HS_SCLL		0x00000015  /* Fast Plus I2C SCL Duty Cycle Low Reg */

#define TRUE				1
#define FALSE				0

extern void I2C_IRQHandler(void);
extern uint32_t I2CInit(uint32_t I2cMode);
extern uint32_t I2CStart(void);
extern uint32_t I2CStop(void);
extern uint32_t I2CEngine(void);

#endif /* end __I2C_H */
/****************************************************************************
 **                            End Of File
 *****************************************************************************/
