/*
[ * testCommand1.c
 *
 * Created: 7/24/2023 12:06:51 PM
 *  Author: greg
 */ 

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>#include "ADC_uC.h"
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
			else if (  buf[1] == 'r' ) doFlashReadTest();
			else if (  buf[1] == 'e' ) doFlashEraseTest();
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

void doFlashWriteTest(void)
{
	char* testChars = "0123456789";
	char buffer[WRITE_TEST_SIZE];
	uint8_t* buff = (uint8_t*) &buffer;
	uint32_t address = 0;
	
	for (uint8_t i = 0; i < NUM_WRITES; i++)
	{
		for (uint8_t j = 0; j < WRITE_TEST_SIZE; j++)
			buffer[j] = testChars[i];
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
	spi_FlashEraseAllBlocks();
	
}