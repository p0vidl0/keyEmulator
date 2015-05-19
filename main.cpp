/**
 * Toyota ECU key amplifier emulator
 *
 *
 */

#include "global.h"
#include "avr/io.h"
#include "avr/interrupt.h"

#define TX_PIN_NUM 2
#define TX_PIN PIND

#define SIGNAL_PIN_NUM 1
#define SIGNAL_PORT PORTD
#define SIGNAL_DDR DDRD

#define SPI_SET_HIGH() sbi(SIGNAL_PORT,SIGNAL_PIN_NUM)
#define SPI_SET_LOW() cbi(SIGNAL_PORT,SIGNAL_PIN_NUM)

#define MESSAGE_SIZE 17
#define MESSAGE_SET_HIGH 0
#define MESSAGE_SET_LOW 1
#define MESSAGE_SEND 2

u08 messageStatus = MESSAGE_SET_HIGH;
u08 message[MESSAGE_SIZE] = {0x03,0x00,0x7e,0xdd,0x92,0xa5,0x0a,0x00,0x00,0x00,0x00,0x34,0x93,0x7e,0x00,0x00,0xf9};

void _spi_send()
{
	static u08 pointer = 0;
	static u08 i = 0;
	static u08 d = 0;

	if (i >= 8)
	{
		i = 0;
		if (pointer >= MESSAGE_SIZE)
		{
			messageStatus = MESSAGE_SET_HIGH;
			pointer = 0;
			return;
		}
	}
	if (i == 0)
	{
	   d = message[pointer++];
	}
	if (d & 0x01)
	{
		SPI_SET_HIGH();
	}
	else
	{
		SPI_SET_LOW();
	}
	d >>= 1;
	i++;
}

ISR(INT0_vect)
{
	static u08 trigger = 0;
	if (trigger == 0)
	{
		trigger++;
	} else
	{
		if (bit_is_set(TX_PIN,TX_PIN_NUM))
		{
			messageStatus = MESSAGE_SET_LOW;
		} else //if (bit_is_clear(TX_PIN,TX_PIN_NUM))
		{
			messageStatus = MESSAGE_SEND;
		}
	}
}

ISR(INT1_vect)
{
	switch (messageStatus)
		{
		case MESSAGE_SET_HIGH:
			SPI_SET_HIGH();
			break;
		case MESSAGE_SET_LOW:
			SPI_SET_LOW();
			break;
		case MESSAGE_SEND:
			_spi_send();
			break;
		}
}


void ports_init(void)
{
	sbi(SIGNAL_PORT,SIGNAL_PIN_NUM);	// SIGNAL_PIN = 1
	sbi(SIGNAL_DDR,SIGNAL_PIN_NUM);	// SIGNAL_PIN делаем выходом
}

void int_init(void)
{
	MCUCR |= (0 << ISC01) | (1 << ISC00) | (1 << ISC11) | (1 << ISC10); // int 0 - любое изменение, int1 - восходящий фронт
	GICR |= (1 << INT1) | (1 << INT0);
//	cbi(SIGNAL_PORT,2);
//	cbi(SIGNAL_PORT,3);
}

void init(void)
{
	int_init();
	ports_init();
	sei();
}

int main(void)
{
	init();
	while(1)
	{

	}
	return 0;
}
