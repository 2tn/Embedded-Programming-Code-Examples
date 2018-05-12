/*
 * LED.c
 *
 * Created: 2015/06/09 13:26:26
 *  Author: Yusuke
 */ 


#include <avr/io.h>
#include <util/delay.h>

int main(void)
{
	DDRD = 0b00000001;
	while(1) {
		PORTD = 0b00000001;
		_delay_ms(100);
		PORTD = 0b00000000;
		_delay_ms(100);
	}
}