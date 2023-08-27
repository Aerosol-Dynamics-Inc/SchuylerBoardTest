/*
 * spi_DACs.c
 *
 * Created: 7/12/2023 2:37:49 PM
 *  Author: greg
 * 
 * Code for two DACS.  First is the "OPTICS DAC" This controls the Optics settings and the Xvitech Pump--Pump code in file....XXXXXXXXXXXXXX
 * 
 *
 * Do all writing in write Through Mode. (i.e. update an output for each register immediately when written to).
 *  
 *  Dac has more capabilities
 *
 */ 


#include <avr/io.h>
#include <stdio.h>
#include "EnablesAndCSs.h"
#include "InstrumentAndDataSettings.h"
#include "spi.h"


// these defines are special commands which are on Bits 12 thru 15
#define HI_Z_OUTPUT 0xD0			// for power down mode
#define WRITE_THROUG_MODE  0x90     //WTM vs WRM (write register mode); WTM changes output immediately on chanllel selected. WRM changes when 
#define SET_ALL_CHANNELS 0xC0     // i.e. broad cast mode--  data on lower 12 bits   

void spi_buckCtrlDac(uint8_t channel, uint16_t value);

void initDACs(void)
{
	// Set Write Through Mode for BuckCtrlDAC
	selectBuckCtrlDAC();
	spiTransferByte(WRITE_THROUG_MODE);   // 
	spiTransferByte(0x00);
	deSelectBuckCtrlDAC();	
	
	// Set Write Through Mode for Optics DAC
	selectOpticsDAC();
	spiTransferByte(WRITE_THROUG_MODE);   //
	spiTransferByte(0x00);
	deSelectOpticsDAC();
	
	spi_buckCtrlDac(0, 4095);   // BUCKS off when control set to 5V
	spi_buckCtrlDac(1, 4095);
	spi_buckCtrlDac(2, 4095);
	spi_buckCtrlDac(3, 4095);
	spi_buckCtrlDac(4, 4095);
	spi_buckCtrlDac(5, 4095);
	spi_buckCtrlDac(6, 0);	
	spi_buckCtrlDac(7, 0);
	
	// Set all channels to zero Volts.
	selectOpticsDAC();
	spiTransferByte(SET_ALL_CHANNELS);   //
	spiTransferByte(0x00);
	deSelectOpticsDAC();	
}




/***********************************************
	spiTransferByte(WRITE_THROUG_MODE);   //
	spiTransferByte(0x00);
	deSelectBuckCtrlDAC();
}