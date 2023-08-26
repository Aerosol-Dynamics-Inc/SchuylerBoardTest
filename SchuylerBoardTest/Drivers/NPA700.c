/*
 * NPA700.c
 *
 * Created: 8/4/2023 4:35:54 PM
 *  Author: greg
 */ 
#include <stdio.h>
#include <util/delay_basic.h>
#include <avr/pgmspace.h>
#include "Globals.h"
#include "NPA700.h"
#include "TWI_MUX.h"
#include "twi_utils.h"


#define NPA700_I2C_ADDR  0x28

typedef enum  { ABS_15PSI, DIFF_1PSI, DIFF_5IN, DIFF_10IN } pressureRange ;  



typedef struct {
	uint16_t pressure;
	uint16_t temp;		
//	uint8_t  validFlag;
	pressureRange  pressureType ;
	uint8_t  muxChannel;
} npa700;


static npa700 npaFlow, npaAbsP, npaU47;  // u47 normally not stuffed

uint8_t readNPA700(npa700 sensor);


void initPressureSensors(void)
{
	npaFlow.pressureType = DIFF_1PSI;
	npaFlow.muxChannel = MUX_CH_DIFF_1PSI;
	readDiff_CH();

	npaAbsP.muxChannel = MUX_CH_ABS;	
	npaAbsP.pressureType = ABS_15PSI; 

	npaU47.muxChannel= MUX_CH_U47;
	npaU47.pressureType = DIFF_1PSI;
		
		
	//setI2Cmux(MUX_SENSOR2_PRESSURE);
	readNPA700(npaFlow);
 	DELAY_5ms; 	DELAY_5ms; 	DELAY_5ms; 	DELAY_5ms; 	DELAY_5ms; 	DELAY_5ms; 	DELAY_5ms;
	readNPA700(npaAbsP);
}
void readDiff_CH(void)
{
	printf("flowc = %d", npaFlow.muxChannel );
}
void readAbs_CH(void)
{
	printf("abs ch = %d", npaAbsP.muxChannel );
}


void readDiffPress(void)
{
	readNPA700(npaFlow);		
}

void readABSPress(void)
{
	readNPA700(npaAbsP);
}

void readU47(void)
{
	readNPA700(npaU47);
}

/******************************************************************************
*                            READ NPA700 PRESSURE                             *
*******************************************************************************
* Description: Reads the NPA700 pressure sensor and save raw value to global.
*
*      Global: static uint16_t raw_pressure[2] - save raw pressure values here
*
*   Arguments: None
*
*      Return: None
******************************************************************************/
uint8_t readNPA700(npa700 sensor)
{
	uint8_t b[4] = {0, 0, 0, 0};
	
	//	PORTJ |= 0x10;
	
//	if ( !((which==ABS_PRESS) || (which==DIFF_PRESS)) )
//	{
//		printf_P(PSTR("sensor unknown\n"));
//		return RETURN_ERROR;
//	}
	printf ("muxChannel = %d \n", sensor.muxChannel);
	setI2Cmux(sensor.muxChannel);  
	printf ("muxChannel = %d \n", sensor.muxChannel);
	twi_read_bytes(NPA700_I2C_ADDR  , b, 4);


	uint8_t diagnostic_bits = b[0]>>6;						  // and 2 MSBs of b[0] are diagnostic bits
	
	if (diagnostic_bits)        {
		if 	    (diagnostic_bits == 1)  printf_P(PSTR("\nSensor %d in Programming Mode\n"),sensor.muxChannel -1);  // 
		else if (diagnostic_bits == 2)  printf_P(PSTR("\nStale Pressure Reading %d\n"),sensor.muxChannel -1);
		else if (diagnostic_bits == 3)  printf_P(PSTR("\nStale Pressure Internal Fault %d\n"),sensor.muxChannel -1);
		return RETURN_ERROR;
	}
	else{
		uint16_t pressureCounts = ( ((b[0] & 0x3F) << 8))  + b[1];  // pressure counts are 14 LSBs of b[0] & b[1],
		uint16_t tempCounts     =  ( (uint16_t)b[2] << 3) + (b[3]>>5); // temperature counts are 11 MSBs of b[2] & b[3]
		printf("%u %u\n", pressureCounts, tempCounts);
		
	}	
	
	return RETURN_OK;
}
