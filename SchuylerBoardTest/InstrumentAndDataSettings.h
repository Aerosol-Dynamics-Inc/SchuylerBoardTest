/*
 * InstrumentAndDataSettings.h
 *
 * Created: 7/5/2023 5:29:02 PM
 *  Author: greg
 */ 


#ifndef INSTRUMENTANDDATASETTINGS_H_
#define INSTRUMENTANDDATASETTINGS_H_

enum { BAUD_115k, BAUD_9600, BAUD_460k } baudRates;

typedef struct  
{
	uint8_t   baud;
	uint16_t wickMINreading;
	uint16_t wickMAXreading;
	
} parameters;




typedef struct
{
	uint16_t  activeADCs;      //     0-3 and 8-15    uC ADCs NOT the for current reading
	uint8_t   activePressSen;  //     normally have 2 but could have 3
	uint8_t   wickSensor;	   //     read only--code sets this when enabling 'in_AdcWick' ADC channel:
	uint8_t   psVolts;         //     set if ADC15 is reading power supply rather than a temperature;
	uint8_t   activeBucks;     //     0 to 5
	uint8_t   activePWMs;      //     0-3
	uint8_t   activeSwitches;  //     0-3  (PWMs and switches can not both be active)
	uint8_t   numPIDs;         //     theoretically we could have six bucks, and four PWMs. 
} configuration;

typedef struct 
{	
	int16_t degC[16];		//  this wastes 8 bytes since  ADC4-ADC7 inputs are used for JTAG
	int16_t wick;			//  wick reading 0-1000.
	uint16_t laserCurr;   
	uint16_t powerSupplyVoltage;
	
	int16_t AbsPress;
	int16_t DiffPress;
	int16_t inputRH;
	int16_t inputTemp;
	int16_t inputDewPoint;	
	uint16_t psVolts;       // in
		
} measured ;



extern parameters     userSettings;
extern configuration  hdwConfig;  // hardwareConfiguration
extern measured		  readings;

#endif /* INSTRUMENTANDDATASETTINGS_H_ */
