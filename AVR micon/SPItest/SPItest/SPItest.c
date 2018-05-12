/*
 * SPItest.c
 *
 * Created: 2015/06/09 13:34:39
 *  Author: Yusuke
 */ 

#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>


#define	USCK	PB7
#define	MOSI	PB5
#define	MISO	PB6


volatile uint8_t triggered = 0;

ISR(PCINT_vect)
{
	if (!bit_is_set(PINB, 4))
	{
		triggered = 1;
	}
}

uint8_t	spi_slave_receive(uint8_t send)
{
	USIDR = send;
	USISR = (1<<USIOIF);
	while (!(USISR & (1<<USIOIF)));	
	uint8_t	receive = USIDR;

	return	receive;
}

void	spi_slave_init()
{
	USICR = (1<<USIWM0)|(1<<USICS1);

	// MISO(PB6)は出力に
	DDRB |= _BV(MISO);
	// MOSI(PB5)/USCKは入力に
	DDRB &= ~(_BV(MOSI) | _BV(USCK));

	// MOSI(PB5)/USCK(PB7)プルアップ
	PORTB |= (_BV(MOSI) | _BV(USCK));
}

int main(void)
{
	spi_slave_init();
	DDRD = 0b00000001;
	
	// PB4プルアップ
	PORTB |= _BV(PB4);

	// PB4の変化割込み
	GIMSK |= _BV(PCIE);
	PCMSK |= _BV(PB4);

	sei();

	uint8_t d = 0x56;
	for (;;)
	{
		while (!triggered)
		{
		}
		triggered = 0;
		
		spi_slave_receive(d);
		
		PORTD = 0b00000001;
		_delay_ms(100);
		PORTD = 0b00000000;
		
	}
}