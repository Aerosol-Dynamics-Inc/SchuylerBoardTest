/*
 * FLASH_Driver.c
 *
 * Created: 8/26/2023 5:02:42 PM
 *  Author: Robin
 */ 

#include <avr/io.h>#include <avr/pgmspace.h>#include <stdio.h>#include <string.h>#include <stdarg.h>//#include <stdbool.h>#include "EnablesAndCSs.h"#include "Globals.h"#include "spi.h"#include "CountersAndTimers.h"#include "spi-nand.h"//#include "external.h"//					              BYTES: Address, Dummy, Data					#define FLASH_RESET 0xFF            //   0,0,0#define FLASH_GET_FEATURES 0x0F     //   1,0,1#define FLASH_SET_FEATURES 0x1F     //   1,0,1 #define FLASH_READ_ID      0x9F     //   0,1,2 #define FLASH_PAGE_READ    0x13     //   3,0,0  Array read#define FLASH_READ_PAGE_CACHE_RANDOM    0x30  // 3,0,0, byte read#define FLASH_READ_PAGE_CACHE_LAST      0x3F  // 0,0,0, byte read#define FLASH_READ_FROM_CACHEx1			0x03// or 0x0B  2,1, 1-2176#define FLASH_READ_FROM_CACHEx2			0x3B//2,1, 1-2176#define FLASH_READ_FROM_CACHEx4			0x6B//2,1, 1-2176#define FLASH_READ_FROM_CACHE_DUAL_IO	0xBB//2,1, 1-2176#define FLASH_READ_FROM_CACHE_QUAD_IO	0xEB//2,2, 1-2176#define FLASH_WRITE_ENABLE              0x06// 0,0,0
#define FLASH_WRITE_DISABLE             0x04// 0,0,0
#define FLASH_BLOCK_ERASE               0xD8// 3,0,0
#define FLASH_PROGRAM_EXECUTE           0x10// 3,0,0   Array program
#define FLASH_LOADx1                    0x02// 2,0,1-2176
#define FLASH_LOADx4                    0x32// 2,0,1-2176  CAN'T USE
#define FLASH_LOAD_RANDOM_DATAx1        0x84// 2,0,1-2176
#define FLASH_LOAD_RANDOM_DATAx4        0x34// 2,0,1-2176   CAN'T USE
#define FLASH_PERMANENT_BLOCK_LOCK_PROTECTION 0x2C//3,0,0 

// register addresses for feature bits (status)
#define FLASH_FEATURE_REG_BLOCK_LOCK	0xA0
#define FLASH_FEATURE_REG_CONFIG		0xB0
#define FLASH_FEATURE_REG_STATUS		0xC0
#define FLASH_FEATURE_REG_DIE_SELECT	0xD0

// bit masks for feature status register
#define FLASH_STATUS_BIT_OIP			0x01
#define FLASH_STATUS_BIT_WEL			0x02
#define FLASH_STATUS_BIT_EFAIL			0x04
#define FLASH_STATUS_BIT_PFAIL			0x08
#define FLASH_STATUS_BIT_ECCS0			0x10
#define FLASH_STATUS_BIT_ECCS1			0x20
#define FLASH_STATUS_BIT_ECCS2			0x40
#define FLASH_STATUS_BIT_CRSBY			0x80
#define FLASH_MAX_NUM_BLOCKS			2048#define FLASH_MAX_BAD_BLOCKS			40#define FLASH_PAGES_PER_BLOCK			64#define FLASH_PAGE_MAX_BYTES			2048#define FLASH_PAGE_FIRST_SPAREAREA_BYTE 2048#define FLASH_NUM_SPARE_AREA_BYTES		128 #define FLASH_RETRY_MAX					10#define  DELAY_COUNT_MULT				1000     // simulates 1 ms maybe?#define  DELAY_COUNT					16union fourBytes {	uint32_t word;	uint8_t byte[4];};union twoBytes{	uint16_t word;	uint8_t byte[2];};void spi_FlashDelay(uint16_t count);void spi_FlashReadID(void);uint8_t spi_FlashWaitUntilReady();uint8_t spi_FlashIsBusy(void);uint8_t spi_FlashGetStatusRegister(void);uint8_t spi_FlashGetFeatures(uint8_t fregister);uint8_t spi_FlashSetFeatures(uint8_t fregister, uint8_t features);uint8_t spi_FlashEnableWrite(void);uint8_t spi_FlashDisableWrite(void);uint8_t spi_FlashWriteToPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes);uint8_t spi_FlashReadFromPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes);uint8_t spi_FlashEraseBlock(uint32_t baddress);uint8_t spi_FlashReset(void);void spi_FlashUnlockAllBlocks(void);void spi_FlashDisplayFeatureRegisters(void);uint8_t spi_FlashBlockIsMarkedBad(uint16_t block);uint32_t spi_FlashMaxAddress(void);uint8_t spi_FlashMapAddress(uint32_t address, uint32_t *pageAddress, uint16_t *byteAddress);#pragma GCC push_options
#pragma GCC optimize ("O0")struct spi_slave {
	u8 op_mode_rx;
	u8 op_mode_tx;
	unsigned int max_speed_hz;
	u8 option;
};
struct spi_nand_chip *flash_chip_ptr;static struct spi_slave flash_spi;uint8_t spi_FlashInitialize(void){	uint8_t retval = 0;	//uint16_t block = 0;	int micronRet = 0;		// assume cold start conditions	// Unlock all blocks	//spi_FlashUnlockAllBlocks();		// initialize Micron flash driver	micronRet = spi_nand_init(&flash_spi, &flash_chip_ptr);	// find first non-bad block	/*	for (block = 0; block < FLASH_MAX_NUM_BLOCKS; block++)	{		printf("Block 0x%04x:", block);		if (!spi_FlashBlockIsMarkedBad(block))		{			//break;			//printf(".");			continue;		}	}	*/		// check for presence of bad block table in first good page	// if bad block table found, load it	// if no bad block table found:	//		read all blocks looking for bad block marks, 	//		fill in bad block table in memory as we go	//		when all bad blocks found, write bad block table into first page of first good block	// 	return retval; 	}	#define CHECK_BUFFER_SIZE	16void printbuffer(uint8_t *buff, uint8_t size){	for (int i=0; i<size; i++)		printf(" %02x",buff[i]);		printf("\n");	}uint8_t spi_FlashBlockIsMarkedBad(uint16_t block){	uint8_t retval = false;	uint8_t buffer[CHECK_BUFFER_SIZE];	uint32_t pageAddr = block * (uint32_t) FLASH_PAGES_PER_BLOCK;	uint16_t byteAddress = (uint16_t) FLASH_PAGE_FIRST_SPAREAREA_BYTE;		if (spi_FlashReadFromPage(pageAddr, byteAddress, buffer, (uint8_t) CHECK_BUFFER_SIZE) == CHECK_BUFFER_SIZE)	{		if (buffer[0] == 0)		{			retval = true;			printf("FLASH- Bad block found at block addr 0x%04x\n", block);		}	}	else	{		printf("Failure attempting to read bad block mark\n");	}	printbuffer(buffer, CHECK_BUFFER_SIZE);	return retval;}// this wait routine should be replaced with a tick counter or other better// method than a stupid tight loopvoid spi_FlashDelay(uint16_t count){	uint32_t loopcnt = count * DELAY_COUNT_MULT;		while (loopcnt--);}//Read Flash ID:  0 address, 1 dummy, 2 Datavoid spi_FlashReadID(void)     {	uint8_t miso[2];		selectFlash();	spiTransferByte(FLASH_READ_ID);	spiTransferByte(0);	miso[0] = spiTransferByte(0);	miso[1] = spiTransferByte(0);	deSelectFlash();	printf("\n0x%02x, 0x%02x\n", miso[0],miso[1]);	}uint8_t spi_FlashWaitUntilReady(){	uint8_t retry = FLASH_RETRY_MAX;	while (spi_FlashIsBusy() && retry)
	{
		spi_FlashDelay(1);
		retry--;
	};	return (retry > 0); 	}/********************************************************************* *  check operation in progress bit (OIP) in feature status register to see if chip is busy. *  returns zero if ready, and 1 if busy with previous command.**********************************************************************/uint8_t spi_FlashIsBusy(void)   {	uint8_t ret = false;		uint8_t status;		status = spi_FlashGetStatusRegister();  // check response	printf("spi_FlashIsBusy: 0x%02x\n", ret);	if (status & (FLASH_STATUS_BIT_OIP | FLASH_STATUS_BIT_CRSBY))		ret = true;	return ret;}uint8_t spi_FlashGetStatusRegister(void){	return 	spi_FlashGetFeatures(FLASH_FEATURE_REG_STATUS);}void spi_FlashDisplayFeatureRegisters(void){	printf("Flash Block Lock    0x%02x\n", spi_FlashGetFeatures(FLASH_FEATURE_REG_BLOCK_LOCK));	printf("Flash Config        0x%02x\n", spi_FlashGetFeatures(FLASH_FEATURE_REG_CONFIG));	printf("Flash Status        0x%02x\n", spi_FlashGetFeatures(FLASH_FEATURE_REG_STATUS));	printf("Flash Die Select    0x%02x\n", spi_FlashGetFeatures(FLASH_FEATURE_REG_DIE_SELECT));	}/********************************************************************* *   *  uint8_t spi_FlashGetFeatures(uint8_t register) *      returns a feature register from FLASH *		register = feature register *      Returns feature register content *       ***********************************************************************/uint8_t spi_FlashGetFeatures(uint8_t fregister){	uint8_t features;			selectFlash();	spiTransferByte(FLASH_GET_FEATURES);	spiTransferByte(fregister);	features = spiTransferByte(0);	deSelectFlash();	return features;}/********************************************************************* *   *  uint8_t spi_FlashGetFeatures(uint8_t register, uint8_t features) *      returns a feature register from FLASH *		register = feature register		features = bits to be set *      Returns feature register content *       ***********************************************************************/uint8_t spi_FlashSetFeatures(uint8_t fregister, uint8_t features){	selectFlash();	spiTransferByte(FLASH_SET_FEATURES);	spiTransferByte(fregister);	spiTransferByte(features);	deSelectFlash();	return spi_FlashGetFeatures(fregister);}void spi_FlashUnlockAllBlocks(void){	uint8_t features;		//features = spi_FlashGetFeatures(FLASH_FEATURE_REG_BLOCK_LOCK);	//printf("spi_FlashUnlockAllBlocks: lock reg starts at 0x%02x\n", features);		spi_FlashSetFeatures(FLASH_FEATURE_REG_BLOCK_LOCK, 0x00);	features = spi_FlashGetFeatures(FLASH_FEATURE_REG_BLOCK_LOCK);	printf("spi_FlashUnlockAllBlocks: lock reg now  0x%02x\n", features);}/********************************************************************* *   *  uint8_t spi_FlashEnableWrite(void) *      send write enable command, then read status register *      Return 1 if write in progress bit (WIP) is not set  *               and write enabled bit (WEL) is set *      Return zero if WIP = 1 or WEL = 0; ***********************************************************************/uint8_t spi_FlashEnableWrite(void)   {	selectFlash();	spiTransferByte(FLASH_WRITE_ENABLE);         	deSelectFlash();	// check if we successful	uint8_t statReg = spi_FlashGetStatusRegister();	//printf("spi_FlashEnableWrite: 0x%02x\n", statReg);			// write enabled (WEL=0) and write not in progress (WIP =0)	return    (statReg & FLASH_STATUS_BIT_WEL)   ? 1 : 0  ;  }uint8_t spi_FlashDisableWrite(void){	selectFlash();	spiTransferByte(FLASH_WRITE_DISABLE);	deSelectFlash();	// check if we successful	uint8_t statReg = spi_FlashGetStatusRegister();	//printf("spi_FlashDisableWrite: 0x%02x\n", statReg);		// write enabled (WEL=0) and write not in progress (WIP =0)	return    (statReg & FLASH_STATUS_BIT_WEL)   ? 0 : 1  ;}/*uint8_t spi_FlashEraseChip(void)  {	if (!spi_FlashEnableWrite())	{		return RETURN_ERROR;	}	selectFlash();	spiTransferByte(FLASH_CHIP_ERASE);	deSelectFlash();	return RETURN_OK;}*//*************************************  Write a data buffer to a Flash page*  uint32_t paddress = page address*  uint16_t address = byte address within page to write buffer*  uint8_t* buffer = pointer to bytes to be written*  uint8_t nbytes = number of bytes to be written*  returns: number of bytes written, or 0 if error**  This function cannot cross a page boundary.  *  That is for a higher level function calling this one to work out************************************/uint8_t spi_FlashWriteRandomToPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes){	uint16_t ret = 0;	uint8_t byte2;	uint8_t byte1;	uint8_t byte0;		printf_P(PSTR("writing page 0x%08lx, byteaddr 0x%04x\n"),paddress,address);			if ((address + nbytes) <=  FLASH_PAGE_MAX_BYTES)	{		if (spi_FlashWaitUntilReady())		{			selectFlash();			byte0 = paddress;			byte1 = (paddress >> 8);			byte2 = (paddress >> 16);			spiTransferByte(FLASH_PAGE_READ);			spiTransferByte(byte2);			spiTransferByte(byte1);			spiTransferByte(byte0);			deSelectFlash();			if (spi_FlashWaitUntilReady())			{				if (spi_FlashEnableWrite())				{					byte0 = address;					byte1 = (address >> 8);					selectFlash();					spiTransferByte(FLASH_LOAD_RANDOM_DATAx1);					spiTransferByte(byte1);					spiTransferByte(byte0);					for (uint8_t i = 0; i < nbytes; i++)					{						spiTransferByte(buffer[i]);					}					deSelectFlash();					if (spi_FlashWaitUntilReady())					{						selectFlash();						byte0 = paddress & 0x000000ff;						byte1 = (paddress >> 8) & 0x000000ff;						byte2 = (paddress >> 16) & 0x000000ff;						spiTransferByte(FLASH_PROGRAM_EXECUTE);						spiTransferByte(byte2);						spiTransferByte(byte1);						spiTransferByte(byte0);						deSelectFlash();						if (spi_FlashWaitUntilReady())						{							if (spi_FlashDisableWrite())								ret = nbytes;							else								printf_P(PSTR("spi_FlashWriteToPage: Flash won't disable write\n"));						}						else							printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after data load\n"));					}					else						printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after data load\n"));				}				else					printf_P(PSTR("spi_FlashWriteToPage: Flash won't enable write\n"));			}			else				printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after page read\n"));		}		else			printf_P(PSTR("spi_FlashWriteToPage: Flash never ready\n"));	}	else		printf_P(PSTR("spi_FlashWriteToPage: buffer crosses page boundary\n"));	return ret;}uint8_t spi_FlashWriteToPage_native(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes){	uint16_t ret = 0;	uint8_t byte2;	uint8_t byte1;	uint8_t byte0;	uint16_t colAddress;		printf_P(PSTR("writing page 0x%08lx, byteaddr 0x%04x\n"),paddress,address);			if ((address + nbytes) <=  FLASH_PAGE_MAX_BYTES)	{		if (spi_FlashWaitUntilReady())		{			if (spi_FlashEnableWrite())			{				// prepare the column address bits, taking the plane select bit from the				// page address (page address bit 6 becomes column address bit 12				colAddress = address;				colAddress |= ((address << 6) & 0x1000);				selectFlash();				byte0 = colAddress;				byte1 = colAddress >> 8;				spiTransferByte(FLASH_LOADx1);				spiTransferByte(byte1);				spiTransferByte(byte0);				for (uint8_t i = 0; i < nbytes; i++)				{					spiTransferByte(buffer[i]);				}				deSelectFlash();				if (spi_FlashWaitUntilReady())				{					byte0 = paddress & 0x00ff;					byte1 = (paddress >> 8) & 0x00ff;					byte2 = (paddress >> 16) & 0x00ff;					selectFlash();					spiTransferByte(FLASH_PROGRAM_EXECUTE);					spiTransferByte(byte2);					spiTransferByte(byte1);					spiTransferByte(byte0);					deSelectFlash();					if (spi_FlashWaitUntilReady())					{						selectFlash();						byte0 = paddress & 0x000000ff;						byte1 = (paddress >> 8) & 0x000000ff;						byte2 = (paddress >> 16) & 0x000000ff;						spiTransferByte(FLASH_PROGRAM_EXECUTE);						spiTransferByte(byte2);						spiTransferByte(byte1);						spiTransferByte(byte0);						deSelectFlash();						if (spi_FlashWaitUntilReady())						{							ret = nbytes;						}						else							printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after data load\n"));					}					else						printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after data load\n"));				}				else					printf_P(PSTR("spi_FlashWriteToPage: Flash won't enable write\n"));			}			else				printf_P(PSTR("spi_FlashWriteToPage: Flash never ready after page read\n"));		}		else			printf_P(PSTR("spi_FlashWriteToPage: Flash never ready\n"));	}	else		printf_P(PSTR("spi_FlashWriteToPage: buffer crosses page boundary\n"));	return ret;}uint8_t spi_FlashWriteToPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes){
	uint64_t longAddress = (paddress * FLASH_PAGE_MAX_BYTES) + address;
	size_t retlen;
	int ret;
	
	ret = spi_nand_write(flash_chip_ptr, (loff_t) longAddress, (size_t) nbytes, &retlen, (const u8 *) buffer);
	
	return (uint8_t) ret;
}uint8_t spi_FlashMapAddress(uint32_t address, uint32_t *pageAddress, uint16_t *byteAddress){	uint8_t retval = false;		// currently this does simple arithmetic ignoring bad blocks 	// soon it will read the bad block table and skip bad blocks	if (address < spi_FlashMaxAddress())	{		*pageAddress =  address / FLASH_PAGE_MAX_BYTES;		*byteAddress =  address % FLASH_PAGE_MAX_BYTES;		retval = true;	}	return retval;}uint32_t spi_FlashMaxAddress(void){	uint32_t maxaddr = (uint16_t) FLASH_MAX_NUM_BLOCKS;	maxaddr *= (uint16_t) FLASH_PAGES_PER_BLOCK;	maxaddr *= (uint16_t) FLASH_PAGE_MAX_BYTES;	return maxaddr;}/*************************************  Write numBytes to Flash*  Crosses page if necessary*************************************/uint8_t spi_FlashWrite(uint32_t address, uint8_t *buffer,  uint8_t numBytes){	uint8_t ret = false;	uint32_t pageAddress;	uint16_t addressInPage;	uint8_t firstPageNumBytes;	uint8_t crossesPage = false;	printf_P(PSTR("Writing to 0x%08lx: %s\n"),address, (const char *) buffer);		if (spi_FlashMapAddress(address, &pageAddress, &addressInPage))	{		if (addressInPage + numBytes > FLASH_PAGE_MAX_BYTES)		{			firstPageNumBytes = FLASH_PAGE_MAX_BYTES - addressInPage;			crossesPage = true;		} 		else		{			firstPageNumBytes = numBytes;		}		if (spi_FlashWriteToPage(pageAddress, addressInPage, buffer, firstPageNumBytes) == firstPageNumBytes)		{			if (crossesPage)			{				if (spi_FlashMapAddress(address+firstPageNumBytes, &pageAddress, &addressInPage))				{					if (spi_FlashWriteToPage(pageAddress, addressInPage, buffer+firstPageNumBytes, numBytes - firstPageNumBytes) == (numBytes - firstPageNumBytes))					{						// success						ret = numBytes;					}					else						printf_P(PSTR("spi_FlashWrite: 2nd page write failed\n"));				}				else				{					printf_P(PSTR("spi_FlashWrite: 2nd page address mapping failed\n"));				}			}			else			{				ret = numBytes;			}		}		else			printf_P(PSTR("spi_FlashWrite: 1st page write failed\n"));	}	return ret;}/*************************************  Read a data buffer from a Flash page*  uint32_t paddress = page address*  uint16_t address = byte address within page to read *  uint8_t* buffer = pointer to buffer to receive bytes read*  uint8_t nbytes = number of bytes to be read*  returns: number of bytes read, or 0 if error**  This function cannot cross a page boundary.  *  That is for a higher level function calling this one to work out************************************/uint8_t spi_FlashReadFromPage_native(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes){	uint16_t ret = 0;	uint8_t byte0; 	uint8_t byte1;	uint8_t byte2;	printf_P(PSTR("\nreading from page 0x%08lx, byteaddr 0x%04x\n"), paddress, address);			if ((address + nbytes) <= (FLASH_PAGE_MAX_BYTES + FLASH_NUM_SPARE_AREA_BYTES))	{		if (spi_FlashWaitUntilReady())		{			selectFlash();			byte0 = paddress & 0x000000ff;			byte1 = (paddress >> 8) & 0x000000ff;			byte2 = (paddress >> 16) & 0x000000ff;			spiTransferByte(FLASH_PAGE_READ);			spiTransferByte(byte2);			spiTransferByte(byte1);			spiTransferByte(byte0);			deSelectFlash();			if (spi_FlashWaitUntilReady())			{				byte0 = address & 0x00ff;				byte1 = (address >> 8) & 0x00ff;				selectFlash();				spiTransferByte(FLASH_READ_FROM_CACHEx1);				spiTransferByte(byte1);				spiTransferByte(byte0);				spiTransferByte(0);  // dummy byte required				for (uint8_t i = 0; i < nbytes; i++)				{					buffer[i] = spiTransferByte(0);				}				deSelectFlash();				ret = nbytes;			}			else				printf_P(PSTR("spi_FlashReadFromPage: Flash never ready after page read\n"));		}		else			printf_P(PSTR("spi_FlashReadFromPage: Flash never ready\n"));	}	else		printf_P(PSTR("spi_FlashReadFromPage: buffer crosses page boundary\n"));	return ret;}uint8_t spi_FlashReadFromPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes){	uint64_t longAddress = (paddress * FLASH_PAGE_MAX_BYTES) + address;
	size_t retlen;
	int ret;
	ret = spi_nand_read(flash_chip_ptr, (loff_t) longAddress, (size_t) nbytes,
	&retlen, (u8 *) buffer);	return (uint8_t) ret;
}/*************************************  Read numBytes from Flash*  Crosses page boundary if necessary*************************************/uint8_t spi_FlashRead(uint32_t address, uint8_t *buffer,  uint8_t numBytes){	uint8_t ret = 0;	uint32_t pageAddress = address / FLASH_PAGE_MAX_BYTES;	uint16_t addressInPage = address % FLASH_PAGE_MAX_BYTES;	uint8_t firstPageNumBytes;	uint8_t crossesPage = false;		if (addressInPage + numBytes > FLASH_PAGE_MAX_BYTES)	{		firstPageNumBytes = (addressInPage + numBytes) - FLASH_PAGE_MAX_BYTES;		crossesPage = true;	} 	else	{		firstPageNumBytes = numBytes;	}    if (spi_FlashReadFromPage(pageAddress, addressInPage, buffer, firstPageNumBytes) == firstPageNumBytes)	{		if (crossesPage)		{			if (spi_FlashReadFromPage(pageAddress+1, 0, buffer+firstPageNumBytes, numBytes - firstPageNumBytes) == (numBytes - firstPageNumBytes))			{				// success				ret = numBytes;			}			else				printf_P(PSTR("spi_FlashRead: 2nd page read failed\n"));		}		else		{			// success			ret = numBytes;		}	}	else		printf_P(PSTR("spi_FlashRead: 1st page read failed\n"));	return ret;}/************************************  Erase a Flash block*  uint32_t baddress = block address************************************/uint8_t spi_FlashEraseBlock(uint32_t baddress){	uint16_t ret = 0;	union fourBytes blockAddress;		blockAddress.word = baddress;		if (baddress <  FLASH_MAX_NUM_BLOCKS)	{		if (spi_FlashWaitUntilReady())		{			if (spi_FlashEnableWrite())			{				selectFlash();				spiTransferByte(FLASH_BLOCK_ERASE);				spiTransferByte(blockAddress.byte[2]);				spiTransferByte(blockAddress.byte[1]);				spiTransferByte(blockAddress.byte[0]);				deSelectFlash();				if (spi_FlashWaitUntilReady())				{					if (spi_FlashDisableWrite())						ret = 1;					else						printf_P(PSTR("spi_FlashEraseBlock: Write diasble failed\n"));				}				else					printf_P(PSTR("spi_FlashEraseBlock: Flash never ready after block erase\n"));			}			else				printf_P(PSTR("spi_FlashEraseBlock: Flash won't enable write\n"));		}		else			printf_P(PSTR("spi_FlashEraseBlock: Flash never ready\n"));	}	else		printf_P(PSTR("spi_FlashEraseBlock: Address exceeds available blocks\n"));	return ret;}uint8_t spi_FlashEraseAllBlocks(){	uint8_t ret = true;	uint16_t blockNum = 0;	uint32_t blockAddr;	uint16_t numBlocks = FLASH_MAX_NUM_BLOCKS;		spi_FlashReset();	spi_FlashUnlockAllBlocks();			for (blockNum = 0; blockNum < numBlocks; blockNum++ )	{		blockAddr = blockNum;		printf_P(PSTR("Erasing block %d\n"),blockNum);		if (!spi_FlashEraseBlock(blockAddr))		{			printf_P(PSTR("spi_FlashEraseAllBlocks: Erase failure at block %d\n"),blockNum);			ret = false;		}	}	return ret;}uint8_t spi_FlashReset(void){	uint8_t status;			selectFlash();	spiTransferByte(FLASH_RESET);	deSelectFlash();	status = spi_FlashGetStatusRegister();	printf("spi_Reset: status is 0x%02x\n", status);		return status; }/*************************************  Interfaces for Micron *  Low-level driver code*************************************/#define SPI_XFER_BEGIN		0x01	/* Assert CS before transfer */
#define SPI_XFER_END		0x02	/* Deassert CS after transfer */
int spi_MT_flash_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout, void *din, unsigned long flags){	uint16_t numBytes = bitlen/8;	uint16_t byteCount;		if (flags & SPI_XFER_BEGIN)		selectFlash();	if(dout)	{		for (byteCount = 0; byteCount < numBytes; byteCount++)		{			spiTransferByte(((uint8_t *) dout)[byteCount]);		}	}	if (din)	{		for (byteCount = 0; byteCount < numBytes; byteCount++)		{			((uint8_t *) din)[byteCount] = spiTransferByte(0);		}	}	if (flags & SPI_XFER_END)		deSelectFlash();				return 0;}//uint8_t MT_flash_paramPageBuffer[256 * 3];uint8_t MT_flash_chipBuff[sizeof(struct spi_nand_chip)];uint8_t MT_flash_oobBuff[128];uint8_t MT_flash_paramPageBuffer[2];//uint8_t MT_flash_chipBuff[2];//uint8_t MT_flash_oobBuff[2];void *spi_MT_flash_allocate_mem(size_t size){	void *retPtr;		if (size == 256 * 3)		retPtr = MT_flash_paramPageBuffer;	else if (size == sizeof(struct spi_nand_chip))		retPtr = MT_flash_chipBuff;	else if (size == 128)		retPtr = MT_flash_oobBuff;	return retPtr;	}unsigned long long spi_MT_flash_get_ticks(void){	return counter1024Hz;}void spi_MT_flash_udelay(unsigned int us){	uint16_t startTicks = counter1024Hz;	while ((counter1024Hz - startTicks) < us)	{		for (int i = 0; i < 1000; i++);	}}#pragma GCC pop_options


