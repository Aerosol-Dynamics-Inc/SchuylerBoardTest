/*
 * spi_FRAM.c
 *
 * Created: 7/12/2023 2:40:23 PM
 *  Author: greg
 */ 
#include <avr/io.h>
#include <stdio.h>
#include "spi.h"
#include "spi_FRAM.h"
#include "EnablesAndCSs.h"

#define SN_ADDRESS 0


// OPCODES for chip  see Table 1 on page 6 of spec
#define FRAM_WR_ENABLE 0x06   // Write Enable
#define FRAM_WR_DIS 0x04   // Write disable  NOT USED
#define FRAM_RDSR 0x05   // Read Status Register
#define FRAM_WRSR 0x01   // Write Status Register
#define FRAM_READ 0x03   // Read Memory Data
#define FRAM_WRITE 0x02   // Write Memory Data
#define FRAM_RDID  0xAF

/* bits in Status Register. Other bits are non-functional and should be written as zero.
  WPEN write protect enable bit. IF one the BP1 and BP0 function as follows:
     BP1  BP0
      0    0    NO protection
	  0    1    protect 0x1800 to 0x1FFF  (upper 1/4)		
	  1    0    protect 0x1000 to 0x1FFF  (upper 1/2)		
	  1    1    protect all  0x0000 to 0x1FFF 		
*/
#define WPEN  7  // Write protect enable bit
#define BP1   3   // Protect block 1        
#define BP0   2   // Protect block 0
#define WEL   1   // Write Enable Latch   This is a read only. Must be set with opcode WREN.


#define FRAM_SIZE (8*1024)

uint16_t serialNumber=0;

/******************************************************************************* *  *  send write enable command to FRAM.  CS must go high after byte sent for command to execute. * ********************************************************************************/void spi_FRAM_writeEnable(void) {	 selectFRAM();	 spiTransferByte(FRAM_WR_ENABLE);  	 deSelectFRAM(); }/******************************************************************************* *   * ********************************************************************************/  uint8_t FRAM_ReadStatusRegister(void) {	 uint8_t i;	 	 selectFRAM();	 spiTransferByte(FRAM_RDSR);  // send Read Status Register opcode	 	 i = spiTransferByte(0);  //   	 deSelectFRAM();	 printf("0x%02X\n",i);	 return i;  }/******************************************************************************* *  *  Write numBytes to FRAM  * ********************************************************************************/void spi_FRAM_write(uint16_t address, uint8_t *mosi, uint16_t numBytes){			if (address + numBytes > FRAM_SIZE)	{ 		printf("FW ERROR, writing past end of FRAM\n");	}			spi_FRAM_writeEnable();     // may not need this after written once	selectFRAM();		spiTransferByte(FRAM_WRITE);    // send Write opcode;	spiSend2ByteBigEndian(address);	spiSendBytes(mosi, numBytes);	deSelectFRAM();}/******************************************************************************* *  *  Read numBytes fram FRAM.  * ********************************************************************************/void spi_FRAM_read(uint16_t address, uint8_t *miso, uint16_t numBytes){	selectFRAM();	//	spi_chipSelect(CSFRAM);	spiTransferByte(FRAM_READ);    // send Read opcode;	spiTransferByte( (uint8_t) (address>>8));    // MSB first	spiTransferByte( (uint8_t) (address & 0xFF) );		//	SPCR &=  ~_BV(SPE);   	//disable SPI hardware for bit-bang read	//	DISABLE_INTERRUPTS;	for (uint16_t i=0;i<numBytes;i++)	{		//#if BOARD_REVISION_C			// Board REV C needs bit bang SPI for FRAM.		//		*miso++ = FRAMgetByte();		//#else		*miso++ = spiTransferByte(0);		//#endif	}//	spi_chipDeSelect(CSFRAM);	deSelectFRAM();	//	ENABLE_INTERRUPTS;	//	SPCR |=  _BV(SPE);  // enable SPI}/*void spi_FRAM_read(uint16_t address, uint8_t *miso, uint16_t numBytes){	selectFRAM();		spiTransferByte(FRAM_READ);    // send Read opcode;	spiSend2ByteBigEndian(address);	spiGetBytes(miso, numBytes) ;	deSelectFRAM();}*/void read_FRAM_DeviceID(void){	uint8_t miso[4];	selectFRAM();	spiTransferByte(FRAM_RDID);    	spiGetBytes(miso, 4);	deSelectFRAM();	printf("%X : %X : %X : %X\n", miso[0],miso[1],miso[2], miso[3]);		}void wrFRAM(void){			spi_FRAM_writeEnable();	uint8_t  miso[4], mosi[4];	mosi[0] = 31;	mosi[1] = 65;	mosi[2] = 129;	mosi[3] = 250;	spi_FRAM_write(1024, mosi, 4);	spi_FRAM_read(1024, miso, 4);	printf("%X %X %X %X\n", miso[0],miso[1],miso[2], miso[3]);		}
void framSaveSerialNumber(uint16_t SN)
{
	serialNumber = SN;
	spi_FRAM_write(SN_ADDRESS, (uint8_t *) &serialNumber, 2);
}

void framReadSerialNumber(void)
{
	spi_FRAM_read(SN_ADDRESS,  (uint8_t *)&serialNumber, 2    );
	printf("SN:%u\n", serialNumber);
}
