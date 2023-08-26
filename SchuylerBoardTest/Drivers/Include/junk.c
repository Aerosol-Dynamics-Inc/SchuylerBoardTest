/*
 * junk.c
 *
 * Created: 8/4/2023 2:54:08 PM
 *  Author: greg
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
//#include <stdio.h>


int8_t nextADC(int8_t channel) {
//	printf("%d", channel);
	return channel;
}

void showActiveADCs(void)
{
//	printf("active ADCs: \n");
}


int8_t incADC_MUX(int8_t channel) {
//	printf("%d", channel);
	return ++channel;

}

void init_adc(void){
	;
}

void doAdc(void)
{
	;
}


void startADCcycle(void)
{
	;
}

