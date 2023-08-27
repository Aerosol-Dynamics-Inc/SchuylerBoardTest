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

/*******************************************************************************
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