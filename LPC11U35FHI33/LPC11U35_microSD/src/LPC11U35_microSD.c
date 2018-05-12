/*
 ===============================================================================
 Name        : LPC11U35_microSD.c
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
#include "fatfs/ff.h"
#include "fatfs/diskio.h"
#include "uart.h"

#define	MMC_CD		!((LPC_GPIO->PIN[0] & (1 << 11)) >> 11)	/* Card detect (yes:true, no:false, default:true) */
FATFS fs;
uint64_t count = 0;
uint64_t time[20] = { };
void SysTick_Handler(void) {
	disk_timerproc();
	count++;
}

#define BUFFERSIZE	200
uint8_t dataBuffer[BUFFERSIZE];
uint16_t bufferPointer = 0;
FRESULT f_buffering(FILE *fp, uint8_t *data, uint16_t length) {
	FRESULT fr = FR_OK;
	while (length) {
		dataBuffer[bufferPointer++] = *data;
		data++;
		length--;
		if (bufferPointer == BUFFERSIZE) {
			uint16_t bw;
			fr = f_write(fp, dataBuffer, BUFFERSIZE, &bw);
			fr = f_sync(fp);
			bufferPointer = 0;
			if (bw != BUFFERSIZE)
				return -1;
		}
	}
	return fr;
}

int main(void) {
	SysTick_Config(SystemCoreClock / 10000);
	UART_Initialize(115200);
	FRESULT fr;
	FIL fp;

	fr = f_mount(&fs, "", 0);
	if (fr == FR_OK) {
		fr = f_open(&fp, "test.txt", FA_CREATE_ALWAYS | FA_READ | FA_WRITE);
		if (fr != FR_OK) {
			ResetISR();
		}
	}
	uint8_t data[50] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16,
			17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33,
			34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50 };
	uint8_t i;
	while (1) {
		uint32_t bw;
		time[0] = count;
		if(i){
			fr = f_write(&fp, data, 50, &bw);
		}else{
			fr = f_sync(&fp);
		}
		time[1] = count;
		UART_printf("time %ld fresult %ld\r\n",
				((long) time[1]) - ((long) time[0]), fr);
		i++;
	}
	return 0;
}
