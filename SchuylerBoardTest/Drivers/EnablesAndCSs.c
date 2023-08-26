/*
 * Enables.c
 *
 * Created: 6/29/2023 1:12:50 PM
 *  Author: greg
 */ 
#include <avr/io.h>
#include <stdio.h>
#include "Ports.h"
#include "spi_ADC2518.h"
//static uint8_t buckStates = 0;


	
	
	

/******************************************************************************	
	There are six bucks (not including the +5V and +3.3V for chips on board)

	Labeling here is in order of connectors under normal use 
	(conditioner, initiator, moderator, optics, spare1 and spare2)
	i.e. J2 pins 1-6, J4, then J3 pins 1-4.
 
	Bucks 0, 1 & 2 have a single enable line from the uC, so to turn them on individually
	one has to check the power settings to see if they are enabled. This is enabled through 
	the GPIO6 on the  TLA2518IRTER ADC chip (U14) 

	Buck  3 (nominally Optics Heater) has a line from GPIO7 and from uC. -- LAYOUT MISTAKE!!
	so just use line from uC
	  
	Bucks 4 & 5 are have individual lines straight from the uC
	
	Buck Number--  Header/Pin -- enable  --	NOMINAL assignments  (odd Pin #'s are ground)
	BUCK 0 --	 J2 P2   --	ADC:GPIO/6	 --  Conditioner
	BUCK 1 --	 J2 P4   --	ADC:GPIO/6	 --  Initiator
	BUCK 2 --	 J2 P4   --	 PG0		 --	 Moderator
	BUCK 3 --	 J4 P1   --  PG1		 --  Optics Heater   (amp-modu)
	BUCK 4 --	 J4 P2   --  ADC:GPIO/6  --  Spare 1  
	BUCK 5 --	 J4 P4   --  PL7         --  Spare 2 
	
	
	
		
**********************************************************************************/

 // Buck Enables 0,1 & 3 are done through  GPIO6 on the (current measurement ADC chip)  -- Write via SPI
void enableBuck014(void)  {  ADC_wrGPO(6,1); }       // Bucks 0, 1, & 4: nominally Cond, Init, Spare 1--on GPO-6 
void disableBuck014(void) {  ADC_wrGPO(6,0); }    

void enableBuck2(void)  { 	PORTG |=  _BV(BUCK2_EN);  }     // Nominally the Moderator
void disableBuck2(void) { 	PORTG &= ~_BV(BUCK2_EN);  }	

void enableBuck3(void) { 	PORTG |=  _BV(BUCK3_EN);  }     // Optics Htr
void disableBuck3(void) { 	PORTG &= ~_BV(BUCK3_EN);  }

void enableBuck5(void)  { 	PORTL |=  _BV(BUCK5_EN);  }	    // nominally spare2 
void disableBuck5(void) { 	PORTL &= ~_BV(BUCK5_EN);  } 



void powerUpWickSensor(void)	{  	PORTE &=  ~_BV(WICK_SENSOR_POWER);		}		// PNP  so low is on
void powerDownWickSensor(void)	{  	PORTE |=   _BV(WICK_SENSOR_POWER);		}	


void toggleHeartBeat()		{	PORTL ^=  _BV(HEARTBEAT_LED);			} 
void HeartBeatOn()			{	PORTL |=  _BV(HEARTBEAT_LED);			}
void HeartBeatOff()			{	PORTL &= ~_BV(HEARTBEAT_LED);			}

   /* SPI  CHIP SELECTS-- Active Low EXCEPT for SD CARD!! */
void selectFRAM()			{	PORTB &= ~_BV(SPI_CS_FRAM);				}
void deSelectFRAM()			{	PORTB |=  _BV(SPI_CS_FRAM);				}

void selectOpticsDAC()		{	PORTH &= ~_BV(SPI_CS_OPTICS_DAC);		}
void deSelectOpticsDAC() 	{	PORTH |=  _BV(SPI_CS_OPTICS_DAC);		} 

void selectADC2518()	    {	PORTH &= ~_BV(SPI_CS_CURRENT_ADC);		}
void deSelectADC2518()	{	PORTH |=  _BV(SPI_CS_CURRENT_ADC);		}

void selectBuckCtrlDAC()	{	PORTH &= ~_BV(SPI_CS_BUCK_CTRL_DAC);	}
void deSelectBuckCtrlDAC()	{	PORTH |=  _BV(SPI_CS_BUCK_CTRL_DAC);	}

void selectFlash()			{	PORTH &= ~_BV(SPI_CS_FLASH);			}
void deSelectFlash()		{	PORTH |=  _BV(SPI_CS_FLASH);			}

void deSelectRTC()			{	PORTH |=  _BV(SPI_CS_RTC);				}
void selectRTC()			{	PORTH &= ~_BV(SPI_CS_RTC);				}


void selectSDcard()			{	PORTH |=  _BV(SPI_CS_SDCARD);			}         // SD is active hi!
void deSelectSDcard()		{	PORTH &= ~_BV(SPI_CS_SDCARD);			}		  // SD is active hi!
	

