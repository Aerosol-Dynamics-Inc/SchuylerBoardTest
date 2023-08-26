/* * spi.c * *  SPI  *     Routines to write to and read the chips that use SPI:  RTC, FRAM, FLASH and DAC. *     Due to a design error the FRAM write can use SPI hardware but the FRAM read is done with firmware *     toggling the SCLK and reading the data on PJ7.  * * *  from section 21 in spec *    *  SPCR - SPI control register *		Bit 7, SPIE: enable SPI interrupt.  set low; not using since in master mode.  *		Bit 6, SPE:  enable SPI bus		   set high *		Bit 5, DORD: 1 is LSB first, 0 is MSB first.   RTC is MSB first *		Bit 4, MSTR: 1 Master mode, 0 slave mode       set high *		Bit 3, CPOL: Clock Polarity. set to zero for RTC, leading edge, is rising. *	 	Bit 2, CPHA: Clock Phase.    set to zero, Sampling on leading edge.  *		Bits 1 & 0, SPR1:  sets SCLK frequency.  SCLK is f_osc divided by 4,16,64, or 128 for 0 to 4.  Freq is twice if SPI2X in SPSR is set	   * *	SPSR - SPI status register *		Bit 7, SPIF: SPI interrupt flag.  gets set but no interrupt because not setting SPIE. *      Bit 6, WCOL: write collision flag.  *      Bits 5 to 1: reserved *		Bit 0: SPI2X: * *	SPDR - SPI data register. *          *    * * Created: 4/5/2018 5:32:50 PM *  Author: GSL *  */ #include <avr/interrupt.h>#include <stddef.h>#include <avr/io.h>#include <stdio.h>#include "EnablesAndCSs.h"#include "Globals.h"#include "Ports.h"//#include "FRAM_FM25640B.h"#include "spi.h"	/********************************************************  Initialize SPI bus*	note: DDR is set up in init_ports(): DDRB =  (~_BV(SPI_MISO))	*   enables master mode,  SCLK rising leading edge, leading edge latch data.  *   Freq = f_osc/16 i.e. 921.6 kHz.   --we should be able to go faster if needed**   As shown in Fig31 of the RTC PCF2129 spec: *    MSB first, Leading Edge Rising, sample on leading Edge********************************************************/void init_spi(void){	//  seems things work at 7.3728 Mhz!//	SPSR = 1;   // SPI2X = 1  double speed (in master mode only)//	SPCR =  _BV(SPE) | _BV(MSTR) ;    // f_osc / 2 =  7.3728Mhz	SPSR = 0;   // SPI2X = 0  i.e.not double speed.//	SPCR =  _BV(SPE) | _BV(MSTR) ;    // f_osc / 4 = 3.6864 Mhz	SPCR =  _BV(SPE) | _BV(MSTR) |  _BV(SPR0);    // f_osc / 16 = 921.6 Khz		deSelectFlash();	deSelectFRAM();	deSelectOpticsDAC();	deSelectRTC();	deSelectBuckCtrlDAC();	deSelectADC2518();}/*************************************  single byte SPI transfer;*  ignore input for read, ignore output for write.*************************************/uint8_t spiTransferByte(uint8_t mosi){	uint8_t miso;   	SPDR = mosi;		while (!(SPSR & 0x80))			;      // wait for transmit	miso = SPDR;	return miso;}/*************************************  multiple byte SPI write transfer;*************************************/void spiSendBytes(uint8_t *mosi, uint16_t numBytes){		for (uint16_t n=0; n<numBytes; n++)	{			SPDR = *mosi++;		while (!(SPSR & 0x80))				;      // wait for transmit	}}/*************************************  multiple byte SPI read*************************************/void spiGetBytes(uint8_t *miso, uint16_t numBytes){	for (uint16_t n=0; n<numBytes; n++)	{		SPDR = 0;		while (!(SPSR & 0x80))			;      // wait for transmit		*miso++ = SPDR;	}}/********************************************** * *   Send 4 bytes in Big Endian format:  MSB first! *   the AVR is a little endian, so to send an uint32_t MSB first,  *   the pointer must start at ((uint8_t *)(&address))+3, and decrease address *   Used for Flash Address or any other uint32_t * *   This just transfers only bytes -- no chip select or write command!! *  *********************************************/void spiSend4ByteBigEndian(uint32_t address){		uint8_t *mosi;	mosi = ((uint8_t *)(&address))+3;  // little endian so start at last byte to send MSB first		for (uint8_t n=0; n<4; n++)	{		SPDR = *mosi--;		while (!(SPSR & 0x80))		;      // wait for transmit	}}void spiSend2ByteBigEndian(uint16_t address){		uint8_t *mosi;	mosi = ((uint8_t *)(&address))+1;  // little endian so start at last byte to send MSB first	SPDR = *mosi--;	while (!(SPSR & 0x80))		;      // wait for transmit	SPDR = *mosi;	while (!(SPSR & 0x80))		;      // wait for transmit		}/*************************************  lower Chip select pin. Shouldn't need to check, but just in case.*************************************/void spi_chipSelect(uint8_t whichChip){}/*************************************  Raise Chip select pin. Shouldn't need to check, but just in case.*************************************/void spi_chipDeSelect(uint8_t whichChip){	}