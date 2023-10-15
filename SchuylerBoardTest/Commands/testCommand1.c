/*
[ * testCommand1.c
 *
 * Created: 7/24/2023 12:06:51 PM
 *  Author: greg
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>#include <stdbool.h>#include "ADC_uC.h"
#include "ADC2518.h"
#include "Globals.h"
#include "EnablesAndCSs.h"
#include "InstrumentAndDataSettings.h"
#include "FETs.h"
#include "FLASH_Driver.h"
#include "NPA700.h"
#include "Ports.h"
#include "RTC.h"  // kill this later
#include "spi_ADC2518.h"
#include "spi_FRAM.h"
#include "spi_DACs.h"
#include "RH_T.h"
#include "TWI_MUX.h"
#include "uart.h"
#include "majorGeneral.h"

#pragma GCC push_options
#pragma GCC optimize ("O0")
uint8_t i=1;

 /*
	As, Am, Ac0-Ac5  -- ADC2518
	aa, as, ad  -- uC ADC
	b,B, o,O,  -- Buck power
	c,r -- rtc
	d,e,  -- enable/disable bucks
	F     -- FET state
	f,i,j -- FRAM
	n,s  -- Serial Number
	p,P -- pressure sensors
	w   -- wick enable/disable
	m   -- select/deselect SD card
	M,2   -- I2C Mux
	RS,RR   -- RH_T start conversion and Read
 */

void doFlashWriteTest(void);
void doFlashReadTest(void);
void doFlashEraseTest(void);
void doFlashTestReadAll(void);
void printSong(void);
void testRandomWR(void);
void FlashABTest(void);
void FlashReadBlocksTest(void);
uint16_t test_block = 0;

void processCommand(void)
{
		// else
		if (buf[0] == 'A')	{
			if (buf[1] == 's')		ADC_rdSystemStatus();		
			else if (buf[1] == 'm')	ADC_manualMode();
			else if (buf[1] == 'c')	ADC_ReadManualModeAnalogCh(buf[2] - '0');
			else if (buf[1] == 'o')	ADC_OverSample(buf[2] - '0');   // oversample 2^N   N<7
			else if (buf[1] == 'l')	ADC_OscSpeed(ADC2518_LowSpeedOSC);
			else if (buf[1] == 'h')	ADC_OscSpeed(ADC2518_HiSpeedOSC);
			printf("cmd %s--", buf);
		}
//		else if (buf[0] == 'F')	{
//			FETstate(buf[1] -'0', buf[2]-'0');
//		}	
		else if (buf[0] == 'p')		readABSPress(); 
		else if (buf[0] == 'P')		readDiffPress(); 
		else if (strcmp(buf,"u47")==0)  readU47();

		else if (buf[0] == 'M')		getMuxConfiguration(); 
		else if (buf[0] == '2')		setI2Cmux(atoi(buf+1));
		
		else if (buf[0] == 'R')	{
			if (buf[1] == 'S') startRH_T_conversion();
			if (buf[1] == 'R') readRH_T();
		}
		else if (buf[0] == 'r')		{ 
			RTC_config();
		}
		else if (buf[0] == 'a') {
			if (  buf[1] == 'a' ) showActiveADCs();
			else if (  buf[1] == 's' ) startADCcycle();			
			else if (  buf[1] == 'd' ) doAdc();			
		}
		else if (buf[0] == 'F') {		
			spi_FlashReadID();
			if (  buf[1] == 'w' ) doFlashWriteTest();
			else if (  buf[1] == 'i' ) spi_FlashInitialize();
			else if (  buf[1] == 'r' ) doFlashReadTest();
			else if (  buf[1] == 'a' ) doFlashTestReadAll();
			else if (  buf[1] == 'e' ) doFlashEraseTest();
			else if (  buf[1] == 'u' ) spi_FlashUnlockAllBlocks();
			else if (  buf[1] == 'f' ) spi_FlashDisplayFeatureRegisters();			
			else if (  buf[1] == 's' ) spi_FlashDisplayStatusRegister();
			else if (  buf[1] == 'x' ) spi_FlashReset();
			else if (  buf[1] == 'b' ) setTestBlock(buf);
			else if (  buf[1] == 't' ) testRandomWR();
			else if (  buf[1] == '1' ) FlashABTest();
			else if (  buf[1] == '2' ) FlashReadBlocksTest();
			else if (  buf[1] == '3' ) FlashBasicTest();
			else if (  buf[1] == '4' ) FlashReadBlocksTest_micron();
/*			if (  buf[1] == 'w' )       getFlashStatusReister();
			else if (  buf[1] == 'e' )  spi_FlashEnableWrite();
			else if (  buf[1] == 'd' ) spi_FlashDisableWrite();
*/			
		}
		else if (buf[0] == 'f') {
				if (  buf[1] == 's' ) FRAM_ReadStatusRegister();
				else if (buf[1] == 'i') read_FRAM_DeviceID();
				else if (buf[1] == 'j') wrFRAM();
				else if (buf[1] == 'e') spi_FRAM_writeEnable();
		}
		
		else if (buf[0] == 's') { framSaveSerialNumber(i); printf("~: %d\n",i++); }
		else if (buf[0] == 'n') framReadSerialNumber();
		else if (buf[0] == 'c') RTC_readCLKF();			
		else if (buf[0] == 'e') {
			if (  buf[1] == '0'
			   || buf[1] == '1' 
			   || buf[1] == '4' )  {enableBuck014(); printf("bucks 0,1,4 (c,i,s1) enabled\n");}
			if (buf[1] == '2')  {enableBuck2(); printf("buck2 (Mod) enabled\n");}
			if (buf[1] == '3')	{enableBuck3();  printf("buck4 (OptHtr) enabled\n");}
			if (buf[1] == '5')	{enableBuck5(); printf("buck5 (spare2) enabled\n");}
		}
		else if (buf[0] == 'd') {
			if (buf[1] == '0')  {disableBuck014();printf("bucks 0,1,4 (c,i,s1) disabled\n");}
			if (buf[1] == '2')  {disableBuck2();printf("buck2 (Mod) disabled\n");}
			if (buf[1] == '3')	{disableBuck3();printf("buck3 (OptHtr) disabled\n");}
			if (buf[1] == '5')	{disableBuck5();printf("buck5 (Spare2) disabled\n");}
		}

		else if (buf[0] == 'o') {			uint16_t v = (buf[1]-'0') *511;			if (v>=0 || v< 4096) spi_OpticsDAC_BroadCast(v);   // 511*8  *   = 4088  --> 4.99V ;  1 --> 0.623V; 2-> 1.246V etc.			else printf("incorrect input o0 to o8\n");			printf("v= %d\n", v);		}		else if (buf[0] == 'O') {			uint8_t ch = (buf[1]-'0');			if (ch <8){				uint16_t v = buf[2]-'0' * 511;				if (v>=0 || v< 4096) spi_OpticsDAC(ch,v);   				else printf("incorrect input: Ocv  0<=c<=7; 0<=V<=0\n");				printf("v= 0x%X\n", v);			}		}		else if (buf[0] == 'b') {			printf("WTF\n");			uint16_t v = ((uint16_t)(buf[1]-'0')) * 511;			if (v>=0 || v< 4096) spi_OpticsBuckCtl_BroadCast(v);   // 511*8  *   = 4088  --> 4.99V ;  1 --> 0.623V; 2-> 1.246V etc.			else printf("incorrect input o0 to o8\n");			printf("v= %d\n", v);		}		else if (buf[0] == 'B') {			uint8_t ch = buf[1]-'0';			if (ch <8){				uint16_t v = ((uint16_t)(buf[2]-'0')) * 511;  				if (v>=0 || v< 4096) spi_buckCtrlDac(ch,v);				else printf("incorect input: Ocv  0<=c<=7; 0<=V<=0\n");				printf("v= %d\n", v);			}		}		else if (buf[0]=='w') {			if (buf[1] == '1')  powerUpWickSensor();			if (buf[1] == '0') 	powerDownWickSensor();		}				else if (buf[0]=='m') {				if (buf[1] == '1')  selectSDcard();				if (buf[1] == '0') 	deSelectSDcard();			}		else printf("\nno cmd:%s \n", buf);
		
				
}

#define WRITE_TEST_SIZE 200
#define NUM_WRITES 10
#define SOURCE_STR_SIZE 10

void setTestBlock(char *buffer)
{
	int b = atoi((const char *)(buffer+3));
	printf("Changing block to %d\n",b);
	test_block = b;
}
void doFlashWriteTest(void)
{
	char* testChars = "0123456789";
	char buffer[WRITE_TEST_SIZE];
	uint8_t* buff = (uint8_t*) &buffer;
	uint32_t address = 0;
	
	for (uint8_t i = 0; i < NUM_WRITES; i++)
	{
		for (uint8_t j = 0; j < WRITE_TEST_SIZE; j++)
			buffer[j] = testChars[j % SOURCE_STR_SIZE];
		if (spi_FlashWrite(address, buff, WRITE_TEST_SIZE) != WRITE_TEST_SIZE)
		{
			printf_P(PSTR("FLASH write failed\n"));
		}
		address += WRITE_TEST_SIZE;
	}
	
}

void doFlashReadTest(void)
{
	char buffer[WRITE_TEST_SIZE+1];
	uint8_t* buff = (uint8_t*) &buffer;
	uint32_t address = 0;

	printf("\nReading from FLASH\n");
	
	for (uint8_t i = 0; i < NUM_WRITES; i++)
	{
		for (uint8_t j = 0; j < WRITE_TEST_SIZE; j++) buffer[j] = (char) 0;
		printf("record %d:\n", i);
		if (spi_FlashRead(address, buff, WRITE_TEST_SIZE) != WRITE_TEST_SIZE)
		{
			printf_P(PSTR("FLASH read failed\n"));
		}
		else
		{
			buffer[WRITE_TEST_SIZE] = '\0';
			printf("%s\n",buffer);
		}
		address += WRITE_TEST_SIZE;
	}
	
}

void doFlashEraseTest(void)
{
	spi_FlashEraseBlock(test_block);
	
}

#define FLASH_TEST_BUFF_SIZE 16


#define FLASH_MAX_NUM_BLOCKS			2048#define FLASH_MAX_BAD_BLOCKS			40#define FLASH_PAGES_PER_BLOCK			64#define FLASH_PAGE_MAX_BYTES			2048#define FLASH_PAGE_FIRST_SPAREAREA_BYTE 2048#define FLASH_NUM_SPARE_AREA_BYTES		128
void doFlashTestReadAll(void)
{
	uint16_t block;
	uint32_t page;
	uint16_t baddr;
	uint8_t buff[FLASH_TEST_BUFF_SIZE];
	
	for (block = test_block; block < FLASH_MAX_NUM_BLOCKS; block++)
	{
		page = block * FLASH_PAGES_PER_BLOCK;
		for (page = block * FLASH_PAGES_PER_BLOCK; page < ((block  * FLASH_PAGES_PER_BLOCK) +1); page++)
		{
			printf("\nB= 0x%04x P= 0x%08lx:", block, page);
			for (baddr = 0; baddr < (FLASH_PAGE_MAX_BYTES); baddr += FLASH_TEST_BUFF_SIZE)
			{
				if (spi_FlashReadFromPage(page, baddr, buff, FLASH_TEST_BUFF_SIZE, false) != FLASH_TEST_BUFF_SIZE)
				{
					printf("Bad read at 0x%04x ", baddr);
				}
				else
				{
					for (int i = 0; i < FLASH_TEST_BUFF_SIZE; i++)
					{
						//printf(" %02x", buff[i]);
						if (buff[i] == 0)
						{
							printf("Found 00 at 0x%04x ", baddr+i);
						}
					}
				}
				spi_FlashReset();
			}	
		}
	}	
}

void printSong(void)
{
	char buff[MAJORGENERAL_LINE_MAX];
	
	for (int i = 0; i < majorGeneral_num_lines(); i++)
	{
		majorGeneral_get_line(i, buff);
		printf("%s",buff);

	}
}

void testRandomWR(void)
{
	// test swath of memory by writing song lyrics to continuous memory and reading
	// them back and comparing 
	uint8_t buff[MAJORGENERAL_LINE_MAX];
	uint8_t buff2[MAJORGENERAL_LINE_MAX];
	uint16_t startblock = test_block;
	uint32_t testLength;
	uint32_t address;
	uint32_t amountWritten = 0;
	uint32_t amountRead = 0;
	int songline = 0;
	int linelen;

	// write the song to the selected block line by line
	testLength = (uint32_t) FLASH_PAGES_PER_BLOCK * (uint32_t) FLASH_PAGE_MAX_BYTES;
	address = startblock * (uint32_t) FLASH_PAGES_PER_BLOCK * (uint32_t) FLASH_PAGE_MAX_BYTES;
	while (amountWritten < testLength)
	{
		linelen = majorGeneral_get_line_len(songline);
		if ((amountWritten + linelen) > testLength)
			break;
		majorGeneral_get_line(songline++, (char *) buff);
		if (spi_FlashWrite(address, buff,  strlen((char *) buff)) == linelen)
		{
			printf("Wrote to to addr 0x%08lx: %s\n",address, (char *) buff);
			spi_FlashDisplayStatusRegister();			amountWritten += linelen;
			address += linelen;
			if (songline >= majorGeneral_num_lines())
				songline = 0;
		}
		else
		{
			printf("Error writing to addr 0x%08lx, line %d\n",address, linelen);
			amountWritten += linelen;
			address += linelen;
			if (songline >= majorGeneral_num_lines())
				songline = 0;
		}
	}
	// read back and check each line of the song
	address = startblock * (uint32_t) FLASH_PAGES_PER_BLOCK * (uint32_t) FLASH_PAGE_MAX_BYTES;
	songline = 0;
	while (amountRead < testLength)
	{
		linelen = majorGeneral_get_line_len(songline);
		if ((amountRead + linelen) > testLength)
			break;
		majorGeneral_get_line(songline++, (char *) buff2);
		if (spi_FlashRead(address, buff,  linelen) == linelen)
		{
			buff[linelen] = 0;
			if (strcmp((const char *) buff, (const char *) buff2) == 0)
			{
				printf("Read from addr 0x%08lx: PASS\n",address);
				spi_FlashDisplayStatusRegister();			}
			else
			{
				printf("Read from addr 0x%08lx: FAIL\n",address);
				spi_FlashDisplayStatusRegister();				printf("%s\n%s\n",buff,buff2);
			}
			amountRead += linelen;
			address += linelen;
			if (songline >= majorGeneral_num_lines())
				songline = 0;
		}
		else
		{
			printf("Error reading from addr 0x%08lx, line %d\n",address, songline);
			amountRead += linelen;
			address += linelen;
			if (songline >= majorGeneral_num_lines())
				songline = 0;
		}
	}
}

void FlashABTest(void)
{
	// test write of A's to 0ne page, B's to next page
	uint8_t buff[32];
	uint16_t block = test_block;
	uint16_t bblock;
	int i;
	int j;
	int k;

	for (j=0; j<8; j++)
	{
		for( i=0; i<32; i++) buff[i] = (uint8_t) 0;
		spi_FlashReadFromPage((block * FLASH_PAGES_PER_BLOCK)+j, 0, buff, 32, false);
		for( i=0; i<32; i++) printf(" %02x",buff[i]);
		printf("\n");		
	}

	
	for( i=0; i<16; i++) buff[i] = (uint8_t) '1';
	// write to 5 consecutive pages
	for (i=0; i<5; i++)
		spi_FlashWriteToPage((block * FLASH_PAGES_PER_BLOCK) + i, 0, buff, 16, false);
	//for( i=0; i<16; i++) buff[i] = (uint8_t) 0;
	//spi_FlashWriteToPage((block * FLASH_PAGES_PER_BLOCK)+1, 0, buff, 16);
	
	for (k = -1; k < 2; k++)
	{
		bblock = block + k;
		printf("block %d\n",bblock);
		for (j=0; j<8; j++)
		{
			for( i=0; i<32; i++) buff[i] = (uint8_t) 0;
			spi_FlashReadFromPage((block * FLASH_PAGES_PER_BLOCK)+j, 0, buff, 32, false);
			for( i=0; i<32; i++) printf(" %02x",buff[i]);
			printf("\n");		
		}
	}
	
	printf("\n\n");
}

#define TEST_READ_SIZE 32

void FlashReadBlocksTest(void)
{
	// test write of A's to 0ne page, B's to next page
	uint8_t buff[TEST_READ_SIZE];
	uint16_t block = test_block;
	uint32_t page = block * FLASH_PAGES_PER_BLOCK;
	int i;
	int j;

	printf("Native\n");
	for (j=0; j<FLASH_PAGE_MAX_BYTES; j += TEST_READ_SIZE)
	{
		for( i=0; i<TEST_READ_SIZE; i++) buff[i] = (uint8_t) 0;
		spi_FlashReadFromPage(page, j, buff, TEST_READ_SIZE, false);
		printf("page 0x%08lx, addr 0x%04x: ", page, j);
		for( i=0; i<TEST_READ_SIZE; i++) printf("%c",buff[i]);
		printf("\n");		
	}
}

void FlashReadBlocksTest_micron(void)
{
	// test write of A's to 0ne page, B's to next page
	uint8_t buff[TEST_READ_SIZE];
	uint16_t block = test_block;
	uint32_t page = block * FLASH_PAGES_PER_BLOCK;
	int i;
	int j;

	printf("Micron\n");
	for (j=0; j<FLASH_PAGE_MAX_BYTES; j += TEST_READ_SIZE)
	{
		for( i=0; i<TEST_READ_SIZE; i++) buff[i] = (uint8_t) 0;
		spi_FlashReadFromPage_micron(page, j, buff, TEST_READ_SIZE, false);
		printf("page 0x%08lx, addr 0x%04x: ", page, j);
		for( i=0; i<TEST_READ_SIZE; i++) printf("%c",buff[i]);
		printf("\n");		
	}
}

void dumpBuffer(int8_t *buffer, uint8_t len)
{
	for( i=0; i<32; i++) printf(" %02x",buffer[i]);
	printf("\n");		
}
void FlashBasicTest(void)
{
	uint32_t page = 7;
	uint16_t byteAddr = 0;
	uint32_t wholeAddr;
	uint8_t buffer[32];
	char testStr[16] = "0123456789ABCDEF";

	wholeAddr = (page * (uint32_t) FLASH_PAGE_MAX_BYTES) + byteAddr;
	spi_FlashReadFromPage(page, byteAddr, buffer, 32, false);
	printf("Pre-read: ");
	dumpBuffer(buffer,32);
	spi_FlashDisplayFeatureRegisters();
	spi_FlashUnlockAllBlocks();
	spi_FlashReset();
	spi_FlashWriteToPage(page, byteAddr, (uint8_t *) testStr, 16, false);
	spi_FlashDisplayFeatureRegisters();
	spi_FlashReset();
	spi_FlashReadFromPage(page, byteAddr, buffer, 32, false);
	spi_FlashDisplayFeatureRegisters();
	spi_FlashReset();
	printf("Post-read: ");
	dumpBuffer(buffer,32);
	
}

#pragma GCC pop_options

