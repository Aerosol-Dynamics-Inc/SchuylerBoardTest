/*
 * ADC2518.c
 *
 * Created: 8/14/2023 4:21:57 PM
 *  Author: greg
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdio.h>
#include "EnablesAndCSs.h"
#include "spi_ADC2518.h"
#include "ADC2518.h"


static uint8_t gpos=0;   // digital output lines-- in this code we are only using GPIO6 as a digital output. 

void initADC2518(void)
{
	ADC_RegisterWrite(PIN_CFG, PIN_CFG_SETTING);
	ADC_RegisterWrite(GPIO_CFG, GPIO_CFG_SETTING);
	ADC_RegisterWrite(GPIO_DRIVE_CFG, GPIO_DRIVE_CFG_SETTING);
	ADC_wrGPO(6,0);  // disables Bucks 0, 1, & 4
}



void ADC_rdSystemStatus(void)
{
	printf("ADC sys reg = %X\n", ADC_RegisterRead(SYSTEM_STATUS,DUMMY_DATA));
}



// really only need a function for GPIO6 -- the Bucks 0 1 & 4 enable.
void ADC_wrGPO(uint8_t outputPin, uint8_t value)   // gpo6 & value = 1 enables bucks 0,1 & 4; value = 0 disables
{
	if (outputPin >7 || value > 1) {
		printf("error, invalid Channel");
	}
	ADC_RegisterWrite(GPO_VALUE, value ?   gpos | _BV(outputPin)  :   gpos & ~_BV(outputPin));
}


void ADC_OverSample(uint8_t log2Samples)   
{
	if (log2Samples > 7) { printf_P(PSTR("OSR: log2samples > 7")); }
	ADC_RegisterWrite(OSR_CFG, log2Samples);
}

void ADC_CalibrateOffset(uint8_t log2Samples)   
{
	ADC_RegisterWrite(GENERAL_CFG, 0x020);
}



void ADC_OscSpeed(uint8_t speed)   
{
	
	uint8_t opmode_cfg = ADC_RegisterRead(OPMODE_CFG, DUMMY_DATA);
	
	if (speed == ADC2518_HiSpeedOSC)  opmode_cfg  &= ~_BV(OSC_SEL);
	else if (speed == ADC2518_LowSpeedOSC)  opmode_cfg  |= _BV(OSC_SEL);
	else {
		printf_P(PSTR("unknown ADC2518 oscilatorSpeed\n"));
		return;
	}
	ADC_RegisterWrite(OPMODE_CFG, opmode_cfg );
}

//sets manual mode  and converts channel 0, which is available on next read (use ADC_ReadManualModeAnalogCh()). 
void ADC_manualMode(void)
{
	ADC_RegisterWrite(SEQUENCE_CFG, 0); // manual mode	
}

void ADC_manualMode(void);


//  Channel reads ~40mA with no current.
uint16_t ADC_ReadManualModeAnalogCh(uint8_t NextChannel)
{
	//	Manual mode should be selected first
	uint16_t adcRead = ADC_RegisterReadWrite(CHANNEL_SEL,  NextChannel);
	printf ("channel = %u, adc = %X,  %u mA \n", NextChannel, adcRead, (uint16_t) (adcRead * 50000 / 4096.0) );
	return adcRead;
}

