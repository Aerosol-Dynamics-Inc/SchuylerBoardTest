/*
 * spi_ADC.c   -- for TLA2518 12 bit ADC on spi. NOTE THIS IS NOT THE ADC on the uC!!
 *            
 *  inputs 0-5 are the BUCK converter current readings
 *  GPIO6 is an output for the BUCK0,BUCK01 & BUCK04
 *
 * Created: 7/27/2023 2:05:43 PM
 *  Author: greg
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "EnablesAndCSs.h"
#include "spi.h"
#include "spi_ADC2518.h"




// BOR  -- Brown out reset--set on power cycle or by writing 1 to _BV(BOR)  in register XXXXXXXX


  // THIS IS A 24BYTE SEQUENCE
void ADC_RegisterWrite(uint8_t addr,  uint8_t data)
{
	selectADC2518();
	spiTransferByte(REG_WR);
	spiTransferByte(addr);
	spiTransferByte(data);
	deSelectADC2518();
}

uint8_t ADC_RegisterRead(uint8_t addr,  uint8_t data)
{
	uint8_t sDataOut;
	selectADC2518();
	spiTransferByte(REG_RD);
	spiTransferByte(addr);
	spiTransferByte(data);
	deSelectADC2518();
	selectADC2518();
	sDataOut = spiTransferByte(REG_RD);
	spiTransferByte(DUMMY_DATA);
	spiTransferByte(DUMMY_DATA);
	return sDataOut;
}


// to read data
uint16_t ADC_RegisterReadWrite(uint8_t addr,  uint8_t data)
{
	uint16_t ret=0;
	uint8_t  dog=0;
	selectADC2518();
	ret = spiTransferByte(REG_WR)<<8;
	ret += spiTransferByte(addr);
	dog += spiTransferByte(data);
	deSelectADC2518();
	printf("adc2518 = 0x%4X%02X\n", ret,dog);
	return ret;
}