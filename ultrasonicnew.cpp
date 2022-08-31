/*
 * ultrasonic.cpp
 *
 * Created: 01-06-2015 13:03:59
 *  Author: Urvashi
 */ 



#define F_CPU 8000000UL
#include <avr/io.h>
#include <util/delay.h>
void lcd_init();
void disp_cmd(unsigned int cmd);
void disp_data(int cmd);
void disp_num(unsigned int num);
void disp_string(char *str);
int adc_read(int channel);
void trigger();
  
uint16_t getPulseWidth()
{
	uint32_t i,result;
	for(i=0;i<600000;i++)
	{
		if(!(PINC & (1<<PC1))) continue; else break;
	}

	if(i==600000)
		return 0xffff; //Indicates time out

	TCCR1A=0X00;
	TCCR1B=(1<<CS11); //Prescaler = Fcpu/8
	TCNT1=0x00; //Init counter

	for(i=0; i<=600000; i++)
	{
		if(PINC & (1<<PC1))
		{
			if(TCNT1 > 60000) break; else continue;
		}
		else
			break;
	}

	if(i==600000)
		return 0xffff; //Indicates time out

	result=TCNT1;

	TCCR1B=0x00;
	if(result > 60000)
		return 0xfffe; //No obstacle
	else
		//return (result>>1);
		return result;
}

int main()
{
	uint16_t r;
	DDRC=0xfd;
	DDRD=0xff;
	lcd_init();
	while(1)
	{
		trigger();
		r=getPulseWidth();
		
		if(r==0xffff)
		{
			disp_cmd(0x80);
			disp_string("Error        ");
		}
		else if(r==0xfffe)
		{
			disp_cmd(0x80);
			disp_string("Error        ");
		}
		else
		{
			int d;
			d=(r/58.0);
			disp_cmd(0x80);
			disp_string("          cm  ");
			disp_cmd(0x84);
			disp_num(d);
		}
	}
}

void disp_cmd(unsigned int cmd)
{
	PORTD=(cmd & 0XF0);
	PORTD=PORTD + 0X02;
	_delay_ms(2);
	PORTD=PORTD - 0X02;
	PORTD=((cmd<<4)& 0XF0);
	PORTD=PORTD + 0X02;
	_delay_ms(2);
	PORTD=PORTD - 0X02;
}
void disp_data( int cmd)
{
	PORTD=(cmd  & 0XF0);
	PORTD=PORTD + 0X03;
	_delay_ms(2);
	PORTD=PORTD - 0X03;
	PORTD=((cmd <<4)& 0XF0);
	PORTD=PORTD + 0X03;
	_delay_ms(2);
	PORTD=PORTD - 0X03;
}

void disp_string( char *str)
{
	int i=0;
	while (str[i]!='\0')
	{
		disp_data (str[i]);
		i++;
		
	}
}

void disp_num(unsigned int num)
{
	int a=0;
	if (num==0)
	{
		disp_data(48);
	}
	disp_cmd (0X04);
	while (num!=0)
	{
		a=num % 10;
		disp_data(a+48);
		num=num/10;
	}
	disp_cmd (0X06);
}
void lcd_init()
{
	disp_cmd(0X02);
	disp_cmd(0X28);
	disp_cmd(0X0C);
	disp_cmd(0X06);
	disp_cmd(0x01);
}

void trigger()
{
	//DDRC=0xfd;
	PORTC=0x00;
	_delay_us(10);
	PORTC=0x01;
	_delay_us(15);
	PORTC=0x00;
	_delay_us(20);
}