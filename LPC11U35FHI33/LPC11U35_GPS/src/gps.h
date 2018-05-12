/*
 * gps.h
 *
 *  Created on: 2016/06/22
 *      Author: Yusuke
 */

#ifndef GPS_H_
#define GPS_H_

#include "i2c.h"
#include "interrupt.h"

#define NAV_PVT_HEADER	0xB5620107

typedef struct {
	uint64_t iTOW;
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint8_t valid;
	uint8_t fixType;
	uint8_t flags;
	uint64_t lon;
	uint64_t lat;
	uint64_t height;
	uint64_t hMSL;
	uint64_t gSpeed;
	uint64_t heading;
} NAV_PVT;

void UBX_send(uint8_t class, uint8_t id, uint16_t length, uint8_t *data);
void GPS_initialize(void);
uint16_t GPS_fifo_length(void);
uint64_t GPS_read_fifo(uint8_t *data, uint16_t length);
void GPS_read_NAV_PVT(uint8_t *data, NAV_PVT *pvt);

#endif /* GPS_H_ */
