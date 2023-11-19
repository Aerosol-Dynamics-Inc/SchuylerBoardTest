/*
 * FLASH_Driver.c
 *
 * Created: 8/26/2023 5:02:42 PM
 *  Author: Robin
 */ 

#include <avr/io.h>#include <avr/pgmspace.h>#include <stdio.h>#include <string.h>#include <stdarg.h>#include <stdbool.h>#include "EnablesAndCSs.h"#include "Globals.h"#include "spi.h"#include "CountersAndTimers.h"#include "spi-nand.h"//#include "external.h"
enum {
    /* Command definitions (please see datasheet for more details) */

    /* WRITE ENABLE commands */
    SPI_FLASH_MT25Q_INS_WREN        			= 0x06,	/* Write enable */
    SPI_FLASH_MT25Q_INS_WRDI        			= 0x04,	/* Write disable */

    /* RESET commands */
    SPI_FLASH_MT25Q_INS_REN		  			= 0x66,	/* Reset enable */
    SPI_FLASH_MT25Q_INS_RMEM		  			= 0x99,	/* Reset memory */

    /* IDENTIFICATION commands */
    SPI_FLASH_MT25Q_INS_RDID        			= 0x9F,	/* Read Identification */
    SPI_FLASH_MT25Q_INS_RDID_ALT    			= 0x9E,	/* Read Identification (alternative command) */
    SPI_FLASH_MT25Q_INS_MULT_IO_RDID   		= 0xAF, /* Read multiple I/O read id */
    SPI_FLASH_MT25Q_INS_DISCOVER_PARAMETER	= 0x5A, /* Read serial flash discovery parameter */

    /* DATA READ commands */
    SPI_FLASH_MT25Q_INS_READ 					= 0x03, /* Read Data Bytes */
    SPI_FLASH_MT25Q_INS_FAST_READ 			= 0x0B, /* Read Data Bytes at Higher Speed */
    SPI_FLASH_MT25Q_INS_DOFR 					= 0x3B,	/* Dual Output Fast Read */
    SPI_FLASH_MT25Q_INS_DIOFR 				= 0xBB, /* Dual Input/Output Fast Read */
    SPI_FLASH_MT25Q_INS_QOFR 					= 0x6B, /* Quad Output Fast Read */
    SPI_FLASH_MT25Q_INS_QIOFR 				= 0xEB, /* Quad Input/Output Fast Read */
    SPI_FLASH_MT25Q_INS_4READ4D 				= 0xE7, /* Word Read Quad I/O */

    /* DATA READ commands (DTR dedicated instructions) */
    SPI_FLASH_MT25Q_INS_FAST_READDTR 			= 0x0D, /* Read Data Bytes at Higher Speed */
    SPI_FLASH_MT25Q_INS_DOFRDTR 				= 0x3D, /* Dual Output Fast Read */
    SPI_FLASH_MT25Q_INS_DIOFRDTR 				= 0xBD, /* Dual Input/Output Fast Read */
    SPI_FLASH_MT25Q_INS_QOFRDTR 				= 0x6D, /* Quad Output Fast Read */
    SPI_FLASH_MT25Q_INS_QIOFRDTR 				= 0xED, /* Quad Input/Output Fast Read */

    /* DATA READ commands (32-bit address) */
    SPI_FLASH_MT25Q_INS_READ4BYTE 			= 0x13, /* Read Data Bytes */
    SPI_FLASH_MT25Q_INS_FAST_READ4BYTE 		= 0x0C, /* Read Data Bytes at Higher Speed */
    SPI_FLASH_MT25Q_INS_DOFR4BYTE 			= 0x3C, /* Dual Output Fast Read */
    SPI_FLASH_MT25Q_INS_DIOFR4BYTE 			= 0xBC, /* Dual Input/Output Fast Read */
    SPI_FLASH_MT25Q_INS_QOFR4BYTE 			= 0x6C, /* Quad Output Fast Read */
    SPI_FLASH_MT25Q_INS_QIOFR4BYTE 			= 0xEC, /* Quad Input/Output Fast Read */

    /* DATA READ commands (32-bit address in DTR mode) */
    SPI_FLASH_MT25Q_INS_FAST_READDTR4BYTE 	= 0x0E, /* Read Data Bytes at Higher Speed */
    SPI_FLASH_MT25Q_INS_DIOFRDTR4BYTE 		= 0xBE, /* Dual Input/Output Fast Read */
    SPI_FLASH_MT25Q_INS_QIOFRDTR4BYTE 		= 0xEE, /* Quad Input/Output Fast Read */

    /* PROGRAM DATA commands */
    SPI_FLASH_MT25Q_INS_PP 					= 0x02, /* Page Program  */
    SPI_FLASH_MT25Q_INS_DIFP					= 0xA2, /* Dual Input Fast Program  */
    SPI_FLASH_MT25Q_INS_DIEFP 				= 0xD2, /* Dual Input Extended Fast Program */
    SPI_FLASH_MT25Q_INS_QIFP 					= 0x32, /* Quad Input Fast Program */
    SPI_FLASH_MT25Q_INS_QIEFP					= 0x12,	/* Quad Input Extended Fast Program */
    SPI_FLASH_MT25Q_INS_QIEFP_ALT				= 0x38, /* Quad Input Extended Fast Program (alternative command) */

    /* PROGRAM DATA commands (32-bit address) */
    SPI_FLASH_MT25Q_INS_PP4BYTE 				= 0x12, /* Page Program with */
    SPI_FLASH_MT25Q_INS_QIFP4BYTE 			= 0x34, /* Quad Input Fast Program with */
    SPI_FLASH_MT25Q_INS_QIEFP4BYTE 			= 0x3E, /* Quad Input Extended Fast Program */
    SPI_FLASH_MT25Q_INS_SE4BYTE 				= 0xDC, /* Sector Erase with */
    SPI_FLASH_MT25Q_INS_SSE4BYTE 				= 0x21, /* Sub-Sector Erase with */

    /* ERASE DATA commands */
    SPI_FLASH_MT25Q_INS_SE					= 0xD8, /* Sector Erase */
    SPI_FLASH_MT25Q_INS_SSE					= 0x20, /* Sub-Sector Erase */
    SPI_FLASH_MT25Q_INS_SSE32K				= 0x52, /* Sub-Sector Erase for 32KB */
    SPI_FLASH_MT25Q_INS_BE					= 0xC7, /* Bulk Erase */
	SPI_FLASH_MT25Q_INS_DE                    = 0xC4, /* Die Erase */
    SPI_FLASH_MT25Q_INS_BE_ALT				= 0x60, /* Bulk Erase (alternative command) */

    /* RESUME/SUSPEND commands */
    SPI_FLASH_MT25Q_INS_PER 					= 0x7A, /* Program/Erase Resume */
    SPI_FLASH_MT25Q_INS_PES 					= 0x75, /* Program/Erase Suspend */

    /* REGISTER commands */
    SPI_FLASH_MT25Q_INS_RDSR 					= 0x05, /* Read Status */
    SPI_FLASH_MT25Q_INS_WRSR 					= 0x01, /* Write Status */
    SPI_FLASH_MT25Q_INS_RDFSR 				= 0x70, /* Read Flag Status */
    SPI_FLASH_MT25Q_INS_CLRFSR 				= 0x50, /* Clear Flag Status */
    SPI_FLASH_MT25Q_INS_RDNVCR 				= 0xB5, /* Read NV Configuration */
    SPI_FLASH_MT25Q_INS_WRNVCR 				= 0xB1, /* Write NV Configuration */
    SPI_FLASH_MT25Q_INS_RDVCR 				= 0x85, /* Read Volatile Configuration */
    SPI_FLASH_MT25Q_INS_WRVCR 				= 0x81, /* Write Volatile Configuration */
    SPI_FLASH_MT25Q_INS_RDVECR 				= 0x65, /* Read Volatile Enhanced Configuration */
    SPI_FLASH_MT25Q_INS_WRVECR 				= 0x61, /* Write Volatile Enhanced Configuration */
    SPI_FLASH_MT25Q_INS_WREAR 				= 0xC5, /* Write Extended Address */
    SPI_FLASH_MT25Q_INS_RDEAR 				= 0xC8, /* Read Extended Address */
    SPI_FLASH_MT25Q_INS_PPMR 					= 0x68, /* Program Protection Mgmt */
    SPI_FLASH_MT25Q_INS_RDPMR 				= 0x2B, /* Read Protection Mgmt */
    SPI_FLASH_MT25Q_INS_RDGPRR 				= 0x96, /* Read General Purpose Read */

    /* Advanced Sectors Protection Commands */
    SPI_FLASH_MT25Q_INS_ASPRD 				= 0x2D, /* ASP Read */
    SPI_FLASH_MT25Q_INS_ASPP 					= 0x2C, /* ASP Program */
    SPI_FLASH_MT25Q_INS_DYBRD 				= 0xE8, /* DYB Read */
    SPI_FLASH_MT25Q_INS_DYBWR 				= 0xE5, /* DYB Write */
    SPI_FLASH_MT25Q_INS_PPBRD 				= 0xE2, /* PPB Read */
    SPI_FLASH_MT25Q_INS_PPBP 					= 0xE3, /* PPB Program */
    SPI_FLASH_MT25Q_INS_PPBE 					= 0xE4, /* PPB Erase */
    SPI_FLASH_MT25Q_INS_PLBRD 				= 0xA7, /* PPB Lock Bit Read */
    SPI_FLASH_MT25Q_INS_PLBWR 				= 0xA6, /* PPB Lock Bit Write */
    SPI_FLASH_MT25Q_INS_PASSRD 				= 0x27, /* Password Read */
    SPI_FLASH_MT25Q_INS_PASSP 				= 0x28, /* Password Write */
    SPI_FLASH_MT25Q_INS_PASSU 				= 0x29, /* Password Unlock */
    SPI_FLASH_MT25Q_INS_DYBRD4BYTE 			= 0xE0, /* DYB Read with 32-bit Address */
    SPI_FLASH_MT25Q_INS_DYBWR4BYTE 			= 0xE1, /* DYB Write with 32-bit Address */

    /* 4-byte address Commands */
    SPI_FLASH_MT25Q_INS_EN4BYTEADDR 			= 0xB7, /* Enter 4-byte address mode */
    SPI_FLASH_MT25Q_INS_EX4BYTEADDR 			= 0xE9, /* Exit 4-byte address mode */

    /* OTP commands */
    SPI_FLASH_MT25Q_INS_RDOTP					= 0x4B, /* Read OTP array */
    SPI_FLASH_MT25Q_INS_PROTP					= 0x42, /* Program OTP array */

    /* DEEP POWER-DOWN commands */
    SPI_FLASH_MT25Q_INS_ENTERDPD				= 0xB9, /* Enter deep power-down */
    SPI_FLASH_MT25Q_INS_RELEASEDPD			= 0xAB,  /* Release deep power-down */

    /* ADVANCED SECTOR PROTECTION commands */
    SPI_FLASH_MT25Q_ASPRD						= 0x2D, /* Advanced sector protection read */
    SPI_FLASH_MT25Q_ASPP						= 0x2C, /* Advanced sector protection program */
    SPI_FLASH_MT25Q_DYBRD						= 0xE8, /* Dynamic protection bits read */
    SPI_FLASH_MT25Q_DYBWR						= 0xE5, /* Dynamic protection bits write */
    SPI_FLASH_MT25Q_PPBRD						= 0xE2, /* Permanent protection bits read */
    SPI_FLASH_MT25Q_PPBP						= 0xE3, /* Permanent protection bits write */
    SPI_FLASH_MT25Q_PPBE						= 0xE4, /* Permanent protection bits erase */
    SPI_FLASH_MT25Q_PLBRD						= 0xA7, /* Permanent protection bits lock bit read */
    SPI_FLASH_MT25Q_PLBWR						= 0xA6, /* Permanent protection bits lock bit write	*/
    SPI_FLASH_MT25Q_PASSRD					= 0x27, /* Password read */
    SPI_FLASH_MT25Q_PASSP						= 0x28, /* Password write */
    SPI_FLASH_MT25Q_PASSU						= 0x29  /* Password unlock */

};

// register addresses for feature bits (status)
#define FLASH_FEATURE_REG_BLOCK_LOCK	0xA0
#define FLASH_FEATURE_REG_CONFIG		0xB0
#define FLASH_FEATURE_REG_STATUS		0xC0
#define FLASH_FEATURE_REG_DIE_SELECT	0xD0

// bit masks for feature status register
#define FLASH_STATUS_BIT_WIP			0x01
#define FLASH_STATUS_BIT_WEL			0x02
#define FLASH_NUM_DIES					2#define FLASH_NUM_SECTORS				2048#define FLASH_SUBSECTORS_PER_SECTOR		16#define FLASH_PAGES_PER_SUBSECTOR		16#define FLASH_PAGE_MAX_BYTES			256 #define FLASH_READY_RETRY_MAX			500#define  DELAY_COUNT_MULT				1000     // simulates 1 ms maybe?#define  DELAY_COUNT					16union fourBytes {	uint32_t word;	uint8_t byte[4];};union twoBytes{	uint16_t word;	uint8_t byte[2];};void spi_FlashDelay(uint16_t count);void spi_FlashReadID(void);uint8_t spi_FlashWaitUntilReady();uint8_t spi_FlashIsBusy(void);uint8_t spi_FlashGetStatusRegister(void);uint8_t spi_FlashEnableWrite(void);uint8_t spi_FlashDisableWrite(void);uint8_t spi_FlashWriteToPage_mt25q(uint32_t address, uint8_t* buffer, uint8_t nbytes);uint8_t spi_FlashReadFromPage_mt25q(uint32_t address, uint8_t* buffer, uint8_t nbytes);uint8_t spi_FlashReset(void);uint8_t spi_FlashBlockIsMarkedBad(uint16_t block);uint32_t spi_FlashMaxAddress(void);uint8_t spi_FlashMapAddress(uint32_t address, uint32_t *pageAddress, uint16_t *byteAddress);#pragma GCC push_options
#pragma GCC optimize ("O0")struct spi_slave {
	u8 op_mode_rx;
	u8 op_mode_tx;
	unsigned int max_speed_hz;
	u8 option;
};
	uint8_t spi_FlashInitialize(void){	uint8_t retval = 0;	return retval; 	}	#define CHECK_BUFFER_SIZE	16void printbuffer(uint8_t *buff, uint8_t size){	for (int i=0; i<size; i++)		printf(" %02x",buff[i]);		printf("\n");	}// this wait routine should be replaced with a tick counter or other better// method than a stupid tight loopvoid spi_FlashDelay(uint16_t count){	uint32_t loopcnt = count * DELAY_COUNT_MULT;		while (loopcnt--);}//Read Flash ID:  0 command, 3 Datavoid spi_FlashReadID(void)     {	uint8_t miso[3];		selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_RDID);	miso[0] = spiTransferByte(0);	miso[1] = spiTransferByte(0);	miso[2] = spiTransferByte(0);	deSelectFlash();	printf("\n0x%02x, 0x%02x, 0x%02x\n", miso[0],miso[1],miso[2]);	}uint8_t spi_FlashWaitUntilReady(){	uint16_t retry = FLASH_READY_RETRY_MAX;	while (spi_FlashIsBusy() && retry)
	{
		spi_FlashDelay(1);
		retry--;
	};	//if (retry > 0)	//	printf_P(PSTR("Ready in %u cycles\n"), (uint16_t)(FLASH_READY_RETRY_MAX - retry));	return (retry > 0); 	}/********************************************************************* *  check operation in progress bit (OIP) in feature status register to see if chip is busy. *  returns zero if ready, and 1 if busy with previous command.**********************************************************************/uint8_t spi_FlashIsBusy(void)   {	uint8_t ret = false;		uint8_t status;		status = spi_FlashGetStatusRegister();  // check response	//printf("spi_FlashIsBusy: 0x%02x\n", ret);	if (status & (FLASH_STATUS_BIT_WIP))		ret = true;	return ret;}uint8_t spi_FlashGetStatusRegister(void){	uint8_t status;			selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_RDSR);	status = spiTransferByte(0);	deSelectFlash();	return status;}uint8_t spi_FlashGetFlagStatusRegister(void){	uint8_t status;		selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_RDFSR);	status = spiTransferByte(0);	deSelectFlash();	return status;}uint8_t spi_ProgramReady(){	uint8_t flags = spi_FlashGetFlagStatusRegister();		return (flags >> 7) & 0x01;}void spi_FlashDisplayStatusRegister(void){	printf("Flash Status        0x%02x\n", spi_FlashGetStatusRegister());	printf("Flash Flag Status   0x%02x\n", spi_FlashGetFlagStatusRegister());}/********************************************************************* *   *  uint8_t spi_FlashGetFeatures(uint8_t register) *      returns a feature register from FLASH *		register = feature register *      Returns feature register content *       ***********************************************************************//********************************************************************* *   *  uint8_t spi_FlashEnableWrite(void) *      send write enable command, then read status register *      Return 1 if write in progress bit (WIP) is not set  *               and write enabled bit (WEL) is set *      Return zero if WIP = 1 or WEL = 0; ***********************************************************************/uint8_t spi_FlashEnableWrite(void)   {	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_WREN);         	deSelectFlash();	uint8_t statReg = spi_FlashGetStatusRegister();	return    (statReg & FLASH_STATUS_BIT_WEL)   ? 1 : 0  ;  }uint8_t spi_FlashDisableWrite(void){	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_WRDI);	deSelectFlash();	uint8_t statReg = spi_FlashGetStatusRegister();	return    (statReg & FLASH_STATUS_BIT_WEL)   ? 0 : 1  ;}void spi_FlashEnable4ByteAddress(void)   {	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_EN4BYTEADDR);         	deSelectFlash();}void spi_FlashDisable4ByteAddress(void)   {	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_EX4BYTEADDR);         	deSelectFlash();}void spi_FlashSetExtendedAddressRegister(uint8_t seg){	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_WREAR);	spiTransferByte(seg & 0x07);	deSelectFlash();}/*uint8_t spi_FlashEraseChip(void)  {	if (!spi_FlashEnableWrite())	{		return RETURN_ERROR;	}	selectFlash();	spiTransferByte(FLASH_CHIP_ERASE);	deSelectFlash();	return RETURN_OK;}*//*************************************  Write a data buffer to a Flash page*  uint32_t paddress = page address*  uint16_t address = byte address within page to write buffer*  uint8_t* buffer = pointer to bytes to be written*  uint8_t nbytes = number of bytes to be written*  returns: number of bytes written, or 0 if error**  This function cannot cross a page boundary.  *  That is for a higher level function calling this one to work out************************************/uint8_t spi_FlashWriteToPage_mt25q(uint32_t address, uint8_t* buffer, uint8_t nbytes){	uint16_t ret = 0;	uint8_t byte3;	uint8_t byte2;	uint8_t byte1;	uint8_t byte0;		//printf_P(PSTR("writing page address 0x%08lx\n"),address);			if (((address & 0x000000ff) + nbytes) <=  FLASH_PAGE_MAX_BYTES)	{		if (spi_FlashWaitUntilReady())		{			if (spi_FlashEnableWrite())			{				spi_FlashEnable4ByteAddress();				// prepare the column address bits, taking the plane select bit from the				// page address (page address bit 6 becomes column address bit 12				selectFlash();				byte0 = address;				byte1 = address >> 8;				byte2 = address >> 16;				byte3 = address >> 24;				spiTransferByte(SPI_FLASH_MT25Q_INS_PP4BYTE);				spiTransferByte(byte3);				spiTransferByte(byte2);				spiTransferByte(byte1);				spiTransferByte(byte0);				for (uint8_t i = 0; i < nbytes; i++)				{					spiTransferByte(buffer[i]);				}				deSelectFlash();				if (!spi_FlashWaitUntilReady())				{						printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after data load\n"));				}				spi_FlashDisableWrite();				ret = nbytes;			}			else				printf_P(PSTR("spi_FlashWriteToPage: Flash will not enable write\n"));		}		else			printf_P(PSTR("spi_FlashWriteToPage: Flash never ready\n"));	}	else		printf_P(PSTR("spi_FlashWriteToPage: buffer crosses page boundary\n"));	return ret;}uint32_t spi_FlashMaxAddress(void){	uint32_t maxaddr = (uint32_t) ((uint32_t)FLASH_NUM_SECTORS * (uint32_t)FLASH_SUBSECTORS_PER_SECTOR * (uint32_t)FLASH_PAGES_PER_SUBSECTOR * (uint32_t)FLASH_PAGE_MAX_BYTES) - 1;	return maxaddr;}/*************************************  Write numBytes to Flash*  Crosses page if necessary*************************************/uint8_t spi_FlashWrite(uint32_t address, uint8_t *buffer,  uint8_t numBytes){	uint8_t ret = false;	uint32_t pageAddress;	uint16_t addressInPage;	uint8_t firstPageNumBytes;	uint8_t crossesPage = false;	//printf_P(PSTR("Writing to 0x%08lx: %s\n"),address, (const char *) buffer);		if ((address & 0x000000ff) + numBytes >= FLASH_PAGE_MAX_BYTES)	{		firstPageNumBytes = FLASH_PAGE_MAX_BYTES - (address & 0x000000ff);		crossesPage = true;	} 	else	{		firstPageNumBytes = numBytes;	}	if (spi_FlashWriteToPage_mt25q(address, buffer, firstPageNumBytes) == firstPageNumBytes)	{		if (crossesPage)		{			if (spi_FlashWriteToPage_mt25q(address+firstPageNumBytes, buffer+firstPageNumBytes, numBytes - firstPageNumBytes) == (numBytes - firstPageNumBytes))			{				// success				ret = numBytes;			}			else				printf_P(PSTR("spi_FlashWrite: 2nd page write failed\n"));		}		else		{			ret = numBytes;		}	}	else		printf_P(PSTR("spi_FlashWrite: 1st page write failed\n"));	return ret;}/*************************************  Read a data buffer from a Flash page*  uint32_t paddress = page address*  uint16_t address = byte address within page to read *  uint8_t* buffer = pointer to buffer to receive bytes read*  uint8_t nbytes = number of bytes to be read*  returns: number of bytes read, or 0 if error**  This function cannot cross a page boundary.  *  That is for a higher level function calling this one to work out************************************/uint8_t spi_FlashReadFromPage_mt25q(uint32_t address, uint8_t* buffer, uint8_t nbytes){	uint16_t ret = 0;	uint8_t byte0; 	uint8_t byte1;	uint8_t byte2;	uint8_t byte3;	uint16_t maxBytes = FLASH_PAGE_MAX_BYTES;//	printf_P(PSTR("\nreading from byteaddr 0x%08x\n"), address);		if (spi_FlashWaitUntilReady())	{		byte0 = address & 0x000000ff;		byte1 = (address >> 8) & 0x000000ff;		byte2 = (address >> 16) & 0x000000ff;		byte3 = (address >> 24) & 0x000000ff;		spi_FlashEnable4ByteAddress(); 		selectFlash();		spiTransferByte(SPI_FLASH_MT25Q_INS_READ4BYTE);		spiTransferByte(byte3);		spiTransferByte(byte2);		spiTransferByte(byte1);		spiTransferByte(byte0);		for (uint8_t i = 0; i < nbytes; i++)		{			buffer[i] = spiTransferByte(0);		}		deSelectFlash();		ret = nbytes;	}	return ret;}/*************************************  Read numBytes from Flash*  Crosses page boundary if necessary*************************************/uint8_t spi_FlashRead(uint32_t address, uint8_t *buffer,  uint8_t numBytes){	return spi_FlashReadFromPage_mt25q(address,  buffer, numBytes);}/************************************  Erase entire Flash************************************/uint8_t spi_FlashErase(){	uint8_t ret = true;	uint32_t dieAddr;	uint8_t addr[4];		spi_FlashReset();			for (uint32_t dieNum = 0; dieNum < FLASH_NUM_DIES; dieNum++ )	{		dieAddr = dieNum * ((spi_FlashMaxAddress() + 1) / 2);		addr[0] = dieAddr & 0x0000000ff;		addr[1] = (dieAddr >> 8) & 0x0000000ff;		addr[2] = (dieAddr >> 16) & 0x0000000ff;		addr[3] = (dieAddr >> 24) & 0x0000000ff;				spi_FlashEnable4ByteAddress();		spi_FlashSetExtendedAddressRegister((uint8_t)(dieNum * 4));		spi_FlashEnableWrite();		selectFlash();		spiTransferByte(SPI_FLASH_MT25Q_INS_DE);		spiTransferByte(addr[3]);		spiTransferByte(addr[2]);		spiTransferByte(addr[1]);		spiTransferByte(addr[0]);		deSelectFlash();		while(!spi_ProgramReady());		spi_FlashDisableWrite();	}	return ret;}uint8_t spi_FlashReset(void){	uint8_t status;			spi_FlashWaitUntilReady();	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_REN);	deSelectFlash();	spi_FlashWaitUntilReady();	selectFlash();	spiTransferByte(SPI_FLASH_MT25Q_INS_RMEM);	deSelectFlash();	spi_FlashWaitUntilReady();	status = spi_FlashGetStatusRegister();	//printf("spi_Reset: status is 0x%02x\n", status);		return status; }#pragma GCC pop_options


