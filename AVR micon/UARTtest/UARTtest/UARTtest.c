/*
 * UARTtest.c
 *
 * Created: 2015/07/01 20:02:18
 *  Author: Yusuke
 */ 


#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

unsigned char USART_Receive(void)
{
	while(!(UCSRA&(1<<RXC))); /*受信完了待機 */
	return UDR; /*受信ﾃﾞｰﾀ取得 */
}

void USART_Transmit(unsigned char data)
{
	while(!(UCSRA&(1<<UDRE))); /*送信ﾊﾞｯﾌｧ空き待機 */
	UDR=data; /*ﾃﾞｰﾀ送信(送信開始)*/
}

ISR(USART_RX_vect)
{	
	char data = UDR;
	USART_Transmit(data);
}


int main(void)
{
	UBRRH=0;
	UBRRL=8;
	UCSRA|=0b00000010;
	UCSRC=(1<<USBS)|(3<<UCSZ0);
	UCSRB=(1<<RXEN)|(1<<TXEN)|(1<<RXCIE);
	OSCCAL=0x58;
	DDRD = 0b00000100;	
	
	PORTD = 0b00000100;
	_delay_ms(1000);
	PORTD = 0b00000000;
	_delay_ms(100);
	
	sei();
	
	while(1)
    {
    }
}