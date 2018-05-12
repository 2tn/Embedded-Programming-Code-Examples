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
#define DEBUG	0											//0のときはだんだん明るくなるモード,1のときは明るさは固定値
#define VDD		((double)5000)									//AREFが5V


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

uint8_t duty_value=0;								//duty比設定変数	0から255まで　ただし0でも消灯はしない

ISR(TIMER0_COMPA_vect)							//PWM	TIMER割り込みの割り込み関数
{
	#if DEBUG==0											//ここではduty比をだんだんあげるデバックをしている
	if (count%5==0)
	{
		duty_value++;
	}
	count++;
	#endif
	
	if (OCR0A==duty_value)	
	{
		OCR0A=255-duty_value;						//比較レジスタを反転
		PWM_LOW();
	}else{
		OCR0A=duty_value;								//比較レジスタを反転
		PWM_HIGH();
	}
}

int main(void)
{
	USART_initialize();								//USARTの初期化		USARTっていうのはSerial通信のことで、デバッグ用。わかんなかったら無視してもいいよ。
	PWM_initialize();									//PWMの初期化			PWMを使うときはPWM_initialize()→PWM_ON()→duty比設定→PWM_OFF()
																		//LEDをONするとき、LEDをOFFにするときはPWM_OFF()を実行すること
	ADC_initialize();									//ADCの初期化			ADCを使うときはADC_initialize()→ADC_convert(char pin)
	
	USART_Transmit(20);
	DDRC|=(1<<5);											//LEDの初期設定
	sei();														//割り込み許可するおまじない


    while(1)
    {
	    uint16_t var=ADC_convert(0);	//ADCがよんだ値をvarに代入
	    //USART_Transmit(var>>8);				//上位バイトをUSART送信
	    //USART_Transmit(var);					//下位バイトをUSART送信
			get_degree();
			_delay_ms(4000);							//1000msまつ　ヒューズビットでシステムクロックを8分周してるので引数を8000にしないとだめ
																		//8分周しているので動作クロックは8MHz
    }
}

void PWM_initialize(){							//PWMの初期化関数
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

void USART_Transmit(unsigned char data)//USARTの初期化関数
{
	while(!(UCSR0A&(1<<UDRE0)));			//送信ﾊﾞｯﾌｧ空き待機 
	UDR0=data;												//ﾃﾞｰﾀ送信(送信開始)
}

void USART_initialize(){
	UBRR0H=0;
	UBRR0L=103;												//U2X0=1,8MHzのときUBBR=103で9600bps
	UCSR0A|=0b00000010;								//U2X0の設定
	UCSR0C=0b00001110;
	UCSR0B=0b00011000;								//USARTの動作開始
}

void ADC_initialize(){
ADMUX = (0<<REFS0);									//外部基準電圧の選択
ADCSRA =(1<<ADEN)|(1<<ADSC)|(0<<ADPS0);// A/D変換許可、1回目変換開始(調整)、分周率2
}

uint16_t ADC_convert(char pin){
	uint16_t ad;
	ADMUX = pin;											// AD変換入力ピン
	ADCSRA&=~(1<<ADIF);
	ADCSRA|=(1<<ADSC);								// 変換開始
	loop_until_bit_is_set(ADCSRA,ADIF); // 変換完了まで待つ
	ad = ADCL;												// 下位8bit取得
	return ad += (ADCH<<8);						// 上位2bit取得
}

void PWM_HIGH(){										//PWM HIGH時の実行関数
	LED_ON();
}
void PWM_LOW(){											//PWM LOW時の実行関数
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