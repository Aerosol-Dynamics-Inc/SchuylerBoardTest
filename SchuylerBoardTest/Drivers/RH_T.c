/*
 * RH_T.c
 *
 * Read Honeywell HIH-8131-021-001S Relative Humidity and Temperature Sensor and calculate Dew Point
 *
 * Created: 8/7/2023 1:45:20 PM
 *  Author: greg
 */ 


#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include <stdio.h>
#include <util/delay_basic.h>
#include "twi_utils.h"
#include "RH_T.h"
#include "Globals.h"
#include "InstrumentAndDataSettings.h"
#include "RH_T.h"

#define HUMIDITY_ADD  0x27    // i.e. (0x27<<1)  I2C address
#define MAX_RH_ERRORS 10




static int8_t rhStarted =0 ;

void delay_ms(uint16_t ms)
{
	for (uint8_t  i =0; i<ms;i++) {DELAY_1ms;}
}


/*****************************************************************
*
*  Read sensors four times, to ensure they are awake and initialized.  See spec sheet.
*  The measurement cycle duration is typically 36.65 ms.
*
*****************************************************************/
void init_RH()
{
	startRH_T_conversion(); delay_ms(50);	readRH_T(); 	delay_ms(50);
	startRH_T_conversion(); delay_ms(50); 	readRH_T(); 	delay_ms(50);
	startRH_T_conversion(); delay_ms(50); 	readRH_T(); 	delay_ms(50);
	startRH_T_conversion(); delay_ms(50); 	readRH_T();
//	savedMode = settings.pid[MODERATOR].mode;  // done after reading settings.  Only needed if someone types rhreset before an error occurs.
}


void startRH_T_conversion()
{
	if (twi_write_bytes(HUMIDITY_ADD, (void *)NULL,0) == RETURN_OK) {    //write zero bites to address NULL to start conversion
		rhStarted = 1;
		printf("started RH_T Conversion\n");
	}
	else 
		printf("RH_T Start Failed\n");
}

void readRH_T(void)
{
	uint8_t buf[4] = {0,0,0,0};
	int32_t rawRH, rawT;


	if ( rhStarted ) {
		rhStarted = 0;
		if (twi_read_bytes(HUMIDITY_ADD, buf,4) != RETURN_OK) {
			printf("RH_T Start Failed\n");
			return;
		}
	}
	else {
		return;
	}
	
	
	if (buf[0] >> 6 != 0) {
		printf("RH_T Stale Data\n");
	}
	else
	{
		rawRH = ((buf[0] & 0x3F) << 8) | buf[1];  // 14 bit code
		// Relative Humidity in percent is (14-bitCode / 2^14 -2) * 100
		printf("RH = %ld (Tenths of a Percent)\n", (rawRH * 1000) / 16382);  //  relative humidity in tenths of a percent
		
		rawT = (((int16_t)buf[2]) <<6) | (buf[3]>>2);  // 14 bit code
		printf("T = %ld (hundreths of a Degree)\n", (rawT * 16500) / ((1<<14)-2) - 4000);// input Temperature in Hundreths of a degC
	}
}