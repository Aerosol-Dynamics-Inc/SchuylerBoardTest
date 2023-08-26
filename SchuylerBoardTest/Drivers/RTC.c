/*
 * RTC.c
 *
 * Created: 7/19/2023 3:38:29 PM
 *  Author: greg
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <time.h>
#include "CountersAndTimers.h"
#include "spi.h"
#include "spi_RTC.h"




/***************************
 *
 *	RTC -- Real Time Clock Defines
 *
*****************************/
#define RTC_WRITE_CMD 0x20   // OR with 5-bit register address
#define RTC_READ_CMD  0xA0   // OR with 5-bit register address
// RTC register address
#define RTC_CTL1	0x00  //
#define RTC_CTL2	0x01  //
#define RTC_CTL3	0x02  //
#define RTC_SEC	    0x03  // seconds and OSF flag in bit 7.
#define RTC_MIN	    0x04
#define RTC_HRS     0x05
#define RTC_DAY     0x06
#define RTC_WKD     0x07  // week day. Sunday is 0.
#define RTC_MTH     0x08
#define RTC_YR      0x09
// Not using alarm registers, timestamp or watchdog registers.
#define RTC_ALSEC     0x0A  // second alarm
#define RTC_ALMIN     0x0B  // minute  alarm
#define RTC_ALHR      0x0C  // hour  alarm
#define RTC_ALDAY     0x0D  // day  alarm
#define RTC_ALWKD     0x0E  // wkDay alarm
#define RTC_CLKOUT    0x0F  // clock out Control
#define RTC_WDTCTL    0x10  // watchdog timer control
#define RTC_WDTVAL    0x11 // watchdog timer value
// 0x12 to 0x18 are time stamp registers. not using define later
#define RTC_AGEOFF    0x19  // aging offset


#define RTC_32768Hz    0x00
#define RTC_16384Hz    0x01
#define RTC_8192Hz     0x02
#define RTC_4096Hz     0x03
#define RTC_2048Hz     0x02
#define RTC_1024Hz     0x05
#define RTC_1Hz        0x06
#define RTC_OSCILLATOR_STOP_FLAG  7  // bit 7 of seconds register.

#define RTC_INT_FREQ     1024              //0.9765625 msec per count, 1024 is 1 second, from RTC chip on pin int 7
#define RTC_BLF		2    // low battery flag bit in Control Register 3

#define RTC_BCD2DEC(A) ( (  (A)>>4) * 10   + ( (A) & 0x0F ) )   //    RTC register are BCD, but converted_time are decimal.
#define RTC_DEC2BCD(A) ( ( ((A)/10) << 4)  + ( (A) % 10   ) )   //    RTC register are BCD, but converted_time are decimal.




uint8_t RTC_read_data[21];  //Buffer to hold most recent read of the RTC chip
char timeString[21];  // formated local time
time_t unixSeconds;
struct tm converted_time;



void RTC_ReadTime(void);




/**************************************
 * 
 * void RTC_config(void) -- does 2 things;
 *
 *   1) Check and if necessary set, clock out pin to generate a 1024 Hz square wave, 
 *      which generates an interrupt on uC INT7 for system timing. Try to set it up to 20 times 
 *   2) Check if oscillator had stopped and issue warning.
 *	 3) Check if battery voltage is below the threshold (typically 2.5V). Chip stops running at ~1.2V
 *   2) Call RTC_ReadTime(). This is the only time we call it, unless user set the time.
 *
 **************************************/
void RTC_config(void)
{
	uint8_t mosi[1], miso[1], i ; 
	mosi[0] = RTC_1024Hz;
		
	printf("in RTC_config();\n");
	for (i=0;i<20;i++)   	{
//		 printf("read RTC %d   ", i);
		spi_RTC_Read( RTC_CLKOUT, miso , 1 );
		if (miso[0] !=  RTC_1024Hz)    {
			spi_RTC_Write( RTC_CLKOUT, mosi,  1);
			printf("read RTC %d:%d  ", i, miso[0]);
			printf("write: 0x%2X \n", mosi[0]); 			
		}
		else
		   break;
	}
	if (i>=20) 	{ 
		printf("ERROR: clock Frequency not at 1024 HZ!\n"); 
	}
/*	
	//	this only seems to work after a switch reset, not after a power down-- look into this
	spi_RTC_Read( RTC_SEC, miso,  1); 
	if (miso[0] & _BV(RTC_OSCILLATOR_STOP_FLAG) )  	{
		mosi[0] = miso[0] & ~_BV(RTC_OSCILLATOR_STOP_FLAG) ;  
		spi_RTC_Write( RTC_SEC, mosi,  1);
		printf_P(PSTR("ERROR: RTC oscillator had stopped. Time is wrong!\n"));
//		flagError(RTC_STOPPED);
		
	}
	
	
	spi_RTC_Read(RTC_CTL3, miso, 1);
	if (miso[0] & _BV(RTC_BLF))	{
		printf_P(PSTR("WARNING: RTC Low Battery. Time may be wrong!\n"));
//		flagError(LOW_BATTERY);
	}
	
//	printf("readingTime....;\n");	
//	deSelectRTC();
	RTC_ReadTime();
*/
}

void RTC_readCLKF(void)
{
	uint8_t miso[1];

	spi_RTC_Read( RTC_CLKOUT, miso , 1 );
	printf("0x%2X", miso[0]); 
}


/*************************************************
 *
 * read PCR2129, convert generate tm struct: converted_time & time_t: unixSections.
 *
 *************************************************/
void RTC_ReadTime(void)
{
	
	spi_RTC_Read( RTC_SEC, RTC_read_data + RTC_SEC, 7 ); // Read the time and date registers 

	// need to convert from BCD to decimal.				
	converted_time.tm_sec   = RTC_BCD2DEC( RTC_read_data[RTC_SEC] & ~_BV(RTC_OSCILLATOR_STOP_FLAG)  );// ); // bit 7 is OSF flag
	converted_time.tm_min   = RTC_BCD2DEC( RTC_read_data[RTC_MIN] );
	converted_time.tm_hour  = RTC_BCD2DEC( RTC_read_data[RTC_HRS] );
	converted_time.tm_year  = RTC_BCD2DEC( RTC_read_data[RTC_YR]) + 100;   // converted_time reference year is 1900, but PCF2129 reference year is 2000
	converted_time.tm_mon   = RTC_BCD2DEC( RTC_read_data[RTC_MTH] ) - 1;   // converted_time months are numbered 0-11, but PCF2129 months are numbered  1-12
	converted_time.tm_mday  = RTC_BCD2DEC( RTC_read_data[RTC_DAY] );
	converted_time.tm_wday  = RTC_read_data[RTC_WKD];
//	converted_time.tm_isdst = ;  // not used in conversion
//	converted_time.tm_yday  = ;  // not used in conversion
	unixSeconds = mk_gmtime(&converted_time);
	printf("unixSeconds = %lu",unixSeconds  );
}



