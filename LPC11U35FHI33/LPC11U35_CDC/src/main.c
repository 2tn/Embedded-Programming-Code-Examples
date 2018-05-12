/***********************************************************************
 * $Id:: main.c 221 2012-01-13 02:54:53Z usb06052                              $
 *
 * Project: USB device ROM Stack test module
 *
 * Description:
 *     USB Communication Device Class User module.
 *
 ***********************************************************************
 *   Copyright(C) 2011, NXP Semiconductor
 *   All rights reserved.
 *
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
 **********************************************************************/
#include <string.h>
#include "LPC11Uxx.h"

#include "vcom.h"
uint16_t count = 0;
void SysTick_Handler(void) {
	if (count < 10000) {
		count++;
	} else {
		count = 0;
		vcom_printf(
				"12345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890Hello,world! %d\r\n",
				1);
	}
}
int main(void) {
	SysTick_Config(SystemCoreClock / 10000);
	vcom_init();
	while (1) {
	}
}
