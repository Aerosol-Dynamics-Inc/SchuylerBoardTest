/*
 * FETS.c
 *
 *  FETs can be simple on/off switches or set up as PWM outputs.
 *
 * Created: 8/8/2023 5:39:01 PM
 *  Author: greg
 */ 
#include <avr/io.h>
#include <stdio.h>
#include "FETS.h"
#include "InstrumentAndDataSettings.h"
#include "Ports.h"


/*  SWITCH FET STATE  --  */
void FETstate(uint8_t whichFET, uint8_t state)
{
	if (whichFET == 0) {
		if (state==FET_ON) PORTG |= _BV(FET0);
		else PORTG &= ~_BV(FET0);
	}
	if (whichFET == 1) {
		if (state==FET_ON) PORTL |= _BV(FET1);
		else PORTL &= ~_BV(FET1);
	}
	if (whichFET == 2) {
		if (state==FET_ON) PORTL |= _BV(FET2);
		else PORTL &= ~_BV(FET2);
	}
	if (whichFET ==3) {
		if (state==FET_ON) PORTB |= _BV(FET3);
		else PORTB &= ~_BV(FET3);
	}
	else printf("unknow FET %d \n", whichFET);
}

/* Set registers for PWMs  */
void FET_PWM_inti() 
{
	if (hdwConfig.activePWMs & 0x01) {    
		// initialize OCOB for FET0
		TCCR0A = _BV(COM0A1) |  _BV(COM0A0) | _BV(COM0B1)  | _BV( WGM01) | _BV(WGM00);  //Set OC0B fast PWM, pin 1
		TCCR0B =  _BV(CS00) ;      //9usec period
		//  DDRG |= _BV(PG5);            set in ports.c.  portG is all outputs
		OCR0A = 0x1;    //
		OCR0B = 0x1;    //
		TIMSK0 = 0;		
	}
	if (hdwConfig.activePWMs & 0x02) {
		// initialize OC5A for FET1
		TCCR5A |= _BV(COM5A1) ;
		DDRL |= _BV(PL3);             //Conditioner
		OCR5AL = 0x80;   // only using 8 bits
	}
	if (hdwConfig.activePWMs & 0x04) {
		// initialize OC5B for FET2
		TCCR5B |= _BV(COM5B1) ;
		DDRL |= _BV(PL4);             //Conditioner
		OCR5BL = 0x80;   // only using 8 bits
	}
	if (hdwConfig.activePWMs & 0x04) {
		// initialize OCOA for FET3
	}
		
	
	
	
}
