/*
 * CountersAndTimers.c
 *
 * Created: 7/18/2023 12:20:57 PM
 *  Author: greg
 */ 
#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "Ports.h"
#include "Globals.h"
#include "CountersAndTimers.h"

volatile uint16_t counter1024Hz = 0;
volatile uint8_t  oneSecFlag=0, halfSecFlag = 0; 
volatile uint16_t ms_twiCount=0;

void init_timer2(void);

//volatile uint8_t timer2Flag = 0;



void init_CountersIimers(void)
{
	init_timer2();
	EICRB = _BV(ISC71) | _BV(ISC70);  //  INT7 --interupt on rising edge. ()   RTC input
	EIMSK = _BV(INT7);                //  Enable int7                          RTC input
	printf("in init counters and timers\n");
	
}


SIGNAL(INT7_vect)
{
	
/*	DISABLE_INTERRUPTS;
	PORTC ^= _BV(PC7);

	counter1024Hz++;

	if (counter1024Hz == 512) {
		halfSecFlag = 1;
	}

	if (counter1024Hz > 1023) {
		counter1024Hz -=1024;
		oneSecFlag = 1;
		ms_twiCount++;
	}

	ENABLE_INTERRUPTS;
	*/
}

SIGNAL(TIMER2_COMPA_vect)
{
	// dead time count
		DISABLE_INTERRUPTS;
		PORTC ^= _BV(PC7);

		counter1024Hz++;

		if (counter1024Hz == 512) {
			halfSecFlag = 1;
		}

		if (counter1024Hz > 1023) {
			counter1024Hz -=1024;
			oneSecFlag = 1;
			ms_twiCount++;
		}

		ENABLE_INTERRUPTS;


	
}





// Set up  deadtime sample frequency to generate an inturrupt.
void init_timer2(void)
{
	printf("initializing timer2\n");
	CLKPR = 0x80;  //Make sure we don't have the fuse bits lowering the clock frequency
	TCCR2A  =  (_BV(WGM21));   //CTC mode
	TCCR2B  =  (_BV(CS22) );   // divide by 64
	OCR2A   = 224;   // with prescale = 1  Freq = 14745600/ (2 * 64 * (224 + 1)) = 512 Hz, but inturrept generated every compare match so Freq = 1024 Hz
	
//	TCCR2B  =  (_BV(CS20) );  // divide by 1
//	OCR2A   = 127;   // with prescale = 1  Freq = 14745600/ (2 * (127 + 1)) = 57600 KHz, but inturrept generated every compare match so Freq = 115,200 Hz?

	
	TCNT2  = 0;
	TIMSK2 |= 2; // Set mode OCIE2A    enable Timer 2 output compare interrupt
}