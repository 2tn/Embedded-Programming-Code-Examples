/*
 * gps.c
 *
 *  Created on: 2016/06/22
 *      Author: Yusuke
 */

#include "gps.h"

void UBX_send(uint8_t class, uint8_t id, uint16_t length, uint8_t *data) {
	int i;
	uint8_t CK_A = 0, CK_B = 0;
	uint8_t out[length + 8];
	out[0] = 0xB5;
	out[1] = 0x62;
	out[2] = class;
	out[3] = id;
	out[4] = length;
	out[5] = length >> 8;

	for (i = 0; i < length + 4; i++) {
		if (i >= 4) {
			out[i + 2] = *data;
			data++;
		}
		CK_A = CK_A + out[i + 2];
		CK_B = CK_B + CK_A;
	}
	out[length + 6] = CK_A;
	out[length + 7] = CK_B;
	I2C_ublox_Write(out, length + 8);
}

void GPS_initialize(void) {
	int i;
	I2C_Initialize();
	interrupt_initialize();
	Timer32_1_initialize(150);
	SysTick_Config(SystemCoreClock / 2 / 1000);
	uint8_t enable_TP[32] = { 0x00, 0x01, 0x00, 0x00, 0x32, 0x00, 0x00, 0x00,
			0x01, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa0,
			0x00, 0x99, 0x99, 0x99, 0x19, 0x00, 0x00, 0x00, 0x00, 0xEF, 0x00,
			0x00, 0x00 };
	UBX_send(0x06, 0x31, 32, enable_TP);
	for (i = 0; i < 6; i++) {
		for (i = 0; i < 10000; i++)
			;
		uint8_t disable_NMEA[8] =
				{ 0xF0, i, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
		UBX_send(0x06, 0x01, 8, disable_NMEA);
	}
	for (i = 0; i < 10000; i++)
		;
	uint8_t enable_PVT[8] = { 0x01, 0x07, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
	UBX_send(0x06, 0x01, 8, enable_PVT);
}

uint16_t GPS_fifo_length(void) {
	uint8_t data[2];
	I2C_Read(0x42, 0xFD, data, 2);
	uint16_t length = data[0] << 8 | data[1];
	return length;
}
uint64_t GPS_read_fifo(uint8_t *data, uint16_t length) {
	I2C_Read(0x42, 0xFF, data, length);
	uint64_t header = ((data[0] << 24) | (data[1] << 16) | (data[2] << 8)
			| data[3]) & 0xFFFFFFFF;
	return header;
}

void GPS_read_NAV_PVT(uint8_t *data, NAV_PVT *pvt) {
	pvt->iTOW = data[9] << 24 | data[8] << 16 | data[7] << 8 | data[6];
	pvt->year = data[11] << 8 | data[10];
	pvt->month = data[12];
	pvt->day = data[13];
	pvt->hour = data[14];
	pvt->min = data[15];
	pvt->sec = data[16];
	pvt->valid = data[17];
	pvt->fixType = data[26];
	pvt->flags = data[27];
	pvt->lon = data[33] << 24 | data[32] << 16 | data[31] << 8 | data[30];
	pvt->lat = data[37] << 24 | data[36] << 16 | data[35] << 8 | data[34];
	pvt->height = data[41] << 24 | data[40] << 16 | data[39] << 8 | data[38];
	pvt->hMSL = data[45] << 24 | data[44] << 16 | data[43] << 8 | data[42];
	pvt->gSpeed = data[69] << 24 | data[68] << 16 | data[67] << 8 | data[66];
	pvt->heading = data[73] << 24 | data[72] << 16 | data[71] << 8 | data[70];
}
