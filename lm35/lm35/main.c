#ifndef F_CPU
#define F_CPU 1600000UL
#endif
#include <avr/io.h>
#include <util/delay.h>

#include "LCD/lcd.h"

enum state{s1, s2, s3};
enum state ST=s1;


void adc_init()
{
	// AREF = AVcc
	ADMUX = (1<<REFS0);

	// ADC Enable and prescaler of 128
	
	ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

// read adc value
uint16_t adc_read(uint8_t ch)
{
	// select the corresponding channel 0~7
	
	ch &= 0b00000111;  // AND operation with 7
	ADMUX = (ADMUX & 0xF8)|ch;

	// start single conversion
	// write '1' to ADSC
	ADCSRA |= (1<<ADSC);

	// wait for conversion to complete
	// ADSC becomes '0' again
	
	while(ADCSRA & (1<<ADSC));

	return (ADC);
}

int main()
{
	DDRB=0xff; //output
	DDRC=0xFF; //output
	uint16_t adc_result0;
	int temp;
	int far;
	char buffer[10];
	

	// initialize adc and lcd
	adc_init();
	lcd_init(LCD_DISP_ON_CURSOR); //CURSOR

	
	lcd_clrscr();
	lcd_gotoxy(0,0);
	
	

	_delay_ms(50);

	while(1)
	{
		adc_result0 = adc_read(0);      // read adc value at PA0
		
		temp=adc_result0/2.01;   // finding the temperature
		
		
		lcd_gotoxy(0,0);
		itoa(temp,buffer,10);
		lcd_puts("Temp=");   //display temperature
		lcd_puts(buffer);
		lcd_gotoxy(7,0);
		lcd_puts("C");
		far=(1.8*temp)+32;
		lcd_gotoxy(9,0);
		itoa(far,buffer,10);
		lcd_puts(buffer);
		lcd_gotoxy(12,0);
		lcd_puts("F");
		_delay_ms(1000);
		
		if(ST==s1)
		{
			PORTC=(0<<PINC1); //cooler
			PORTB=(0<<PINB1); //heater
			
			lcd_clrscr();
			lcd_home();
			lcd_gotoxy(0,1);
			lcd_puts("Both OFF");

			if(temp>35)
			{
				ST=s2;
				lcd_clrscr();
				lcd_home();
				lcd_gotoxy(0,1);
				lcd_puts("Cooler ON");
				
				PORTC=(1<<PINC1); //cooler
				PORTB=(0<<PINB1); //heater
				
			}
			
			if(temp<15)
			{
				ST=s3;
				lcd_clrscr();
				lcd_home();
				lcd_gotoxy(0,1);
				lcd_puts("Heater ON");
				
				PORTC=(0<<PINC1); //cooler
				PORTB=(1<<PINB1); //heater
			}
		}
		if(ST==s2)
		{
			if(temp<25)
			{
				ST=s1;
				PORTC=(0<<PINC1); //cooler
				PORTB=(0<<PINB1); //heater
			}
			
		}
		if(ST==s3)
		{
			if(temp>30)
			{
				ST=s1;
				PORTC=(0<<PINC1); //cooler
				PORTB=(0<<PINB1); //heater
			}
			
		}

	}
}