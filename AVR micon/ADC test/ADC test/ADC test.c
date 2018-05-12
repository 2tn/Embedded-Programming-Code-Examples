/*
 * ADC_test.c
 *
 * Created: 2015/10/07 20:24:15
 *  Author: Yusuke
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <math.h>

#define LED_ON()	PORTC|=(1<<5);		//LED
#define LED_OFF()	PORTC&=~(1<<5);
#define DEBUG	0											//0�̂Ƃ��͂��񂾂񖾂邭�Ȃ郂�[�h,1�̂Ƃ��͖��邳�͌Œ�l
#define VDD		((double)5000)									//AREF��5V


void USART_initialize();
void PWM_initialize();
void ADC_initialize();
void USART_Transmit(unsigned char data);
uint16_t ADC_convert(char pin);
void PWM_HIGH();
void PWM_LOW();
void PWM_ON();
void PWM_OFF();

uint16_t get_degree();

#if DEBUG==0
uint8_t count=0;
#endif

uint8_t duty_value=0;								//duty��ݒ�ϐ�	0����255�܂Ł@������0�ł������͂��Ȃ�

ISR(TIMER0_COMPA_vect)							//PWM	TIMER���荞�݂̊��荞�݊֐�
{
	#if DEBUG==0											//�����ł�duty������񂾂񂠂���f�o�b�N�����Ă���
	if (count%5==0)
	{
		duty_value++;
	}
	count++;
	#endif
	
	if (OCR0A==duty_value)	
	{
		OCR0A=255-duty_value;						//��r���W�X�^�𔽓]
		PWM_LOW();
	}else{
		OCR0A=duty_value;								//��r���W�X�^�𔽓]
		PWM_HIGH();
	}
}

int main(void)
{
	USART_initialize();								//USART�̏�����		USART���Ă����̂�Serial�ʐM�̂��ƂŁA�f�o�b�O�p�B�킩��Ȃ������疳�����Ă�������B
	PWM_initialize();									//PWM�̏�����			PWM���g���Ƃ���PWM_initialize()��PWM_ON()��duty��ݒ聨PWM_OFF()
																		//LED��ON����Ƃ��ALED��OFF�ɂ���Ƃ���PWM_OFF()�����s���邱��
	ADC_initialize();									//ADC�̏�����			ADC���g���Ƃ���ADC_initialize()��ADC_convert(char pin)
	
	USART_Transmit(20);
	DDRC|=(1<<5);											//LED�̏����ݒ�
	sei();														//���荞�݋����邨�܂��Ȃ�


    while(1)
    {
	    uint16_t var=ADC_convert(0);	//ADC����񂾒l��var�ɑ��
	    //USART_Transmit(var>>8);				//��ʃo�C�g��USART���M
	    //USART_Transmit(var);					//���ʃo�C�g��USART���M
			get_degree();
			_delay_ms(4000);							//1000ms�܂@�q���[�Y�r�b�g�ŃV�X�e���N���b�N��8�������Ă�̂ň�����8000�ɂ��Ȃ��Ƃ���
																		//8�������Ă���̂œ���N���b�N��8MHz
    }
}

void PWM_initialize(){							//PWM�̏������֐�
	TCCR0A  =(1<<WGM01);
	TCCR0B	=0b011;
	OCR0A		=duty_value;
}

void PWM_ON(){
	TIMSK0=(1<<OCIE0A);
}

void PWM_OFF(){
	TIMSK0=0;
}

void USART_Transmit(unsigned char data)//USART�̏������֐�
{
	while(!(UCSR0A&(1<<UDRE0)));			//���M�ޯ̧�󂫑ҋ@ 
	UDR0=data;												//�ް����M(���M�J�n)
}

void USART_initialize(){
	UBRR0H=0;
	UBRR0L=103;												//U2X0=1,8MHz�̂Ƃ�UBBR=103��9600bps
	UCSR0A|=0b00000010;								//U2X0�̐ݒ�
	UCSR0C=0b00001110;
	UCSR0B=0b00011000;								//USART�̓���J�n
}

void ADC_initialize(){
ADMUX = (0<<REFS0);									//�O����d���̑I��
ADCSRA =(1<<ADEN)|(1<<ADSC)|(0<<ADPS0);// A/D�ϊ����A1��ڕϊ��J�n(����)�A������2
}

uint16_t ADC_convert(char pin){
	uint16_t ad;
	ADMUX = pin;											// AD�ϊ����̓s��
	ADCSRA&=~(1<<ADIF);
	ADCSRA|=(1<<ADSC);								// �ϊ��J�n
	loop_until_bit_is_set(ADCSRA,ADIF); // �ϊ������܂ő҂�
	ad = ADCL;												// ����8bit�擾
	return ad += (ADCH<<8);						// ���2bit�擾
}

void PWM_HIGH(){										//PWM HIGH���̎��s�֐�
	LED_ON();
}
void PWM_LOW(){											//PWM LOW���̎��s�֐�
	LED_OFF();
}

uint16_t get_degree(){
	double d[3];
	d[0]=((((double)ADC_convert(2))/1024)*VDD-1650)/660+1;
	d[1]=((((double)ADC_convert(3))/1024)*VDD-1650)/660+1;
	d[2]=((((double)ADC_convert(4))/1024)*VDD-1650)/660+1;
	
	USART_Transmit((int)(d[0]*100));
	USART_Transmit((int)(d[1]*100));
	USART_Transmit((int)(d[2]*100));
	return 0;
}