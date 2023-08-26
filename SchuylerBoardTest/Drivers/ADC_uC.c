
/*
 * ADCs.c
 *
 * Created: 7/6/2023 10:34:48 AM
 *  Author: greg
 */ 


/*
 * ADCs.c
 * 
 *  Read ADCs.  
 *   ADC0 to ADC3, PF0-PF3, are used for Conditioner, Initiator, Moderator and Optics Temperatures 
 *   ADC1 to ADC7 not used--pins dedicated to JTAG
 *   ADC8,  PK0 Heatsink Temp;
 *	 ADC9,  PK1 Case Temperature; 
 *	 ADC10, PK2 Wick Sensor
 *	 ADC11, PK3 Laser Current
 *	 ADC12, PK4 Temperature 0--normally not used. J16 pins 7&8
 *	 ADC13, PK5 Temperature 1--normally not used. J16 pins 5&6
 *	 ADC14, PK6 Temperature 2--normally not used. J16 pins 3&4
 *	 ADC15, PK7 Temperature 3 (J16 pins 3&4) OR Power Supply voltage--Selected by Jumper on J18
 *   
 * Created: 4/19/2018 5:56:39 PM
 *  Author: GSL
 *
 */ 

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "ADC_uC.h"
#include "Globals.h"
#include "IOassignments.h"
#include "InstrumentAndDataSettings.h"
//#include "TemperatureLookUp.h"
//#include "Pids.h"




//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Local variables/flags/#defines.  (If declared static we can't export them)....
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define ENABLE_ADC_INT    ADCSRA |=  (_BV(ADIE))		// enable ADC interrupt 
#define DISABLE_ADC_INT   ADCSRA &= ~(_BV(ADIE))		// disable ADC interrupt 
#define START_CONVERSION  ADCSRA |=  (_BV(ADSC))		// start ADC conversion.


static uint16_t adcReadings[16] = {0};
//static uint8_t  adcCount=0;
uint8_t  adcCompleteFlag = 0;   
static int8_t  firstADCchannel,  currentADCchannel; 


//static uint16_t adcHistory[128];

uint16_t adcAve[16];
int16_t wadc=0;   // for debugging, saved for 'wadc' command
 
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Local functions.....
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//void ConvertReading(int8_t adcChannel);
int8_t incADC_MUX(int8_t channel);
int8_t nextADC(int8_t current);   //  only returns the next active ADC channel.  


/****************************
 *
 *  These are defined to make the ADC ISR 
 *
 * Define activeADCflages[] -- Flags to specify which ADC channels are used, 
 * Define firtADC           -- first active channel.
 * Define nextADC[]           -- 
 *
 ****************************/

// nextADC -- returns the next active channel--IT DOES NOT CHANGE THE ADC MUX

int8_t nextADC(int8_t current)
{
	uint8_t next = current;
	uint8_t i=0;  // used to check maximum of 16 channels
	while (i++<17) {
		 ++next;
		 next %= 16;  // roll over;
		 if (hdwConfig.activeADCs & (1<<next)){
			 break;
		 }
	}
	if (i==17) {
	  printf_P(PSTR("ERROR: No ADC Channels active\n")); 		
	}
	return (i==17 ?  -1 : next);
}




// prints the active ADC channels.  Bits 0-15 correspond to ADC 0-15.


void showActiveADCs(void)
{
	for (uint8_t i=0; i<16;i++){
		if (hdwConfig.activeADCs & (1<<i)){
			 printf_P(PSTR("%d "), i);
		}
	}
	printf_P(PSTR("\n"));
	printf("active ADCs: 0x%02x\n", hdwConfig.activeADCs);
}


// increments MUX to next active channel.

int8_t incADC_MUX(int8_t channel)
{
  // channel is initially the last channel converted;
  // except first time through is set to 15 so it starts at channel 0;
  uint8_t next; 
  if ((next = nextADC(channel)) != -1) {  
	  ADMUX &= ~( _BV( MUX0) | _BV(MUX1) | _BV( MUX2) | _BV( MUX3) | _BV( MUX4) );  // clear lines MUX0 to MUX4.
	  ADMUX |=   next & (_BV( MUX0) | _BV(MUX1) | _BV( MUX2));
	  if (next & 0x08)   // if  (16 > i >= 8)  set MUX5 bit;
		  ADCSRB |= _BV(MUX5);
	  else
		  ADCSRB &= ~_BV(MUX5);
	
  }  

  return next;
}


/////////////////////////////////////////////////////////////////////////////

void init_adc()
{

	//ADMUX  = _BV(REFS0); // set for AVCC, 10 bit accuracy right adjusted result. Channel Zero, port F0
	// ADCSRA bits:
	//7: ADEN -- ADC enable; 
	//6: ADSC -- ADC start conversion;  Writing one starts a conversion
	//5: ADIE -- ADC interrupt enable
	//4: ADATE -- ADC auto Trigger enable; not using. if one triggers selected by ADTS bits in ADCSRB 
	//3: ADIF -- ADC interrupt flag. 1 on completion of conversion
	//2:0 – ADPS2:0: ADC Prescaler Select Bits. These bits determine the division factor between the XTAL frequency and the input clock to the ADC.
	//   divisor is 2^n for ADPS2:0 = n if n 1 to 7; but divisior is 2 for ADPS2:0 = 0.  ADC clock must be between 50kHz and 200 kHz for 10 bit accuracy
	
	firstADCchannel = currentADCchannel =  incADC_MUX(15);  // next after channel 15 is ADC0 

	if (currentADCchannel != -1)  {		
		START_CONVERSION;
		ADCSRA = _BV(ADEN) | _BV(ADIE) | _BV(ADPS2) | _BV(ADPS1)| _BV(ADPS0);  //  enable interrupt, enable ADC, ADC freq is F_CPU / 128 = 115.2 KHz.
		DIDR0 = 0xFF;   // disable digital buffer on pins F0 to F7 to save power.
		DIDR1 = 0xFF;   // disable digital buffer on pins K0 to K7 to save power	
		startADCcycle();		
	} 
	else { 
		printf_P("ERROR: No ADCs Active\n");
	}
	
}


/////////////////////////////////////////////////////////////////////////////
/// adc_int is a flag each bit is set when the corresponding channel has a valid (averaged) reading
//  in this code there are 5 valid channels.
/////////////////////////////////////////////////////////////////////////////



SIGNAL(ADC_vect) 
{ 
  //static uint8_t k=0;	
//  PORTJ ^=0x04;	
  static uint16_t numReadings=0;  
    
  adcReadings[currentADCchannel] +=  ADCL;  // lower byte of adc value;
  adcReadings[currentADCchannel] += ( (uint16_t) ADCH) << 8   ;   // upper byte of adc value;
  printf("%d %02x%02x %d:    ", currentADCchannel, ADCH, ADCH, adcReadings[currentADCchannel]);
 
  // set whichADC to next active channel
  currentADCchannel = incADC_MUX(currentADCchannel);

  // After completing one cycle, don't start next conversion until we can get a local copy of readings  
  if (currentADCchannel == firstADCchannel)   {
	 if (++numReadings == NUM_ADC_SUMS) {
		 adcCompleteFlag = 1;  
		 numReadings = 0;
	 }
  }
  if (!adcCompleteFlag)  {
	START_CONVERSION;
  }
}

/**********************************
 **********************************/

void doAdc(void)
{
	int8_t adcChannel = firstADCchannel;
	
	uint8_t i=0;
	do {
		if (i%4 == 0) printf("\n");
		printf("%d  %u;   ", adcChannel, adcReadings[adcChannel]);
		adcChannel = nextADC(adcChannel);
	} while (adcChannel != firstADCchannel);
}


void startADCcycle(void) 
{
	adcCompleteFlag =0;
	
	int8_t adcChannel = firstADCchannel;
	
	do {
		adcReadings[adcChannel] = 0;
		adcChannel = nextADC(adcChannel);		
	} while (adcChannel != firstADCchannel);
	START_CONVERSION;
}



/*
void ConvertReading(int8_t adcChannel)
{
	if (    (adcChannel< in_AdcWick) 
	     || (adcChannel >= in_Adc12 && adcChannel <=  in_Adc14  ) 
		 || (adcChannel == in_Adc15 && !hdwConfig.psVolts  ) )
	{
		readings.degC[adcChannel] = convertADCtoDegC(adcReadings[adcChannel]);
	} 
	
	else if (adcChannel == in_AdcWick) 
	{
		wadc = adcReadings[adcChannel] >> ADC_SUM_SHIFT;
		readings.wick = 1000L - (wadc - userSettings.wickMINreading) * 1000L  / ((userSettings.wickMAXreading - userSettings.wickMINreading));
	} 

	else if (adcChannel == in_AdcLaserCur) 
	{
			readings.laserCurr = (adcReadings[in_AdcLaserCur] + (NUM_ADC_SUMS>>1)) >> ADC_SUM_SHIFT ;  // 1 bit is approximately 0.5mA. reading is in half of millivolt		
	}
	else if (hdwConfig.psVolts && adcChannel == in_AdcPSV) 
	{
			readings.psVolts = (215 * adcReadings[in_AdcPSV] + (1 << (9 + ADC_SUM_SHIFT)) ) >> (10 + ADC_SUM_SHIFT) ;  // 
	} 
//	else if (adcChannel == 


}
*/