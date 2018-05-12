/*
 ===============================================================================
 Name        : LPC1114FN28_microSDtest.c
 Author      : $(author)
 Version     :
 Copyright   : $(copyright)
 Description : main definition
 ===============================================================================
 */

#ifdef __USE_CMSIS
#include "LPC11xx.h"
#include "fatfs/ff.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

// TODO: insert other include files here

// TODO: insert other definitions and declarations here
FATFS FatFs;
void disk_timerproc();
void SysTick_Handler(void) {
	disk_timerproc();
}

int main(void) {
	volatile static uint32_t period;
	printf("Hello World\n");
	// Force the counter to be placed into memory
	volatile static int i = 0;
	// Enter an infinite loop, just incrementing a counter
	period = SystemCoreClock / 1000;
	SysTick_Config(period);

	FIL fp1; // ファイル オブジェクト //
	FIL fp2;
	FRESULT fr; // 戻り値 //

	//デフォルト ドライブにワークエリアを与える//
	fr = f_mount(&FatFs, "", 1);

	//ファイル書き込みテスト
	fr = f_open(&fp1, "write1.txt", FA_CREATE_ALWAYS);
	f_close(&fp1);
	fr = f_open(&fp1, "write1.txt", FA_WRITE);
	fr = f_printf(&fp1, "Hello, world!!");
	f_close(&fp1);
	//ファイル書き込みテスト終了

	//ファイル読み出しテスト
	char line[82]; // 行バッファ //
	fr = f_open(&fp2, "read.txt", FA_READ);
	if (fr)
		return (int) fr;
	// 1行ずつ読み出して表示 //
	while (f_gets(line, sizeof line, &fp2))
		printf(line);
	// ファイルを閉じる //
	f_close(&fp2);
	//ファイル読み出しテスト終了

	while (1) {
		i++;
	}
	return 0;
}

