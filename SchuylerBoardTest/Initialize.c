/*
 * CFile1.c
 *
 * Created: 6/28/2023 11:22:43 AM
 *  Author: greg
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <avr/interrupt.h>
#include <time.h>
#include "ADC_uC.h"
#include "CountersAndTimers.h"
#include "EnablesAndCSs.h"
#include "Globals.h"
#include "InstrumentAndDataSettings.h"
#include "NPA700.h"
#include "Ports.h"
#include "RTC.h"
#include "spi.h"
#include "spi_DACs.h"
#include "spi_ADC2518.h"
#include "twi_utils.h"
#include "uart.h"



static FILE uart_str = FDEV_SETUP_STREAM(uart0_putchar, uarts_getchar, _FDEV_SETUP_RW);


void 	disablePowerOutputs(void)
{
//	buckEnable(0x3F,0);  // disable all six bucks
//	wickSensorEnable(0);
	// SD power outputs
//	pwmEnable(0x0F,0);  disable all PWMs.
}


void init_Ports(void)
{
	DDRA = DDRA_SETTING;
	DDRB = DDRB_SETTING;
	DDRC = DDRC_SETTING;
	DDRD = DDRD_SETTING;
	DDRE = DDRE_SETTING;
	DDRF = DDRF_SETTING;
	DDRG = DDRG_SETTING;
	DDRH = DDRH_SETTING;
	DDRJ = DDRJ_SETTING;
	DDRK = DDRK_SETTING;	
	DDRL = DDRL_SETTING;
	
	PORTC = 0xFF;
	PORTH = 0xFF;
	
	disablePowerOutputs();	
	deSelectSDcard();
}




void initialize(void)
{
	
	stdout = stdin = &uart_str;
	init_uart(0);	
	init_Ports();

	init_spi();

	init_CountersIimers();
	hdwConfig.activeADCs = 0x8F0F; // 0x8F0F = psv(15), lcur(11), wick(10), case(9), HS(8), Optics(3), mod(2), init(1), cond (0);
//	initDACs();

	for (uint16_t i=0;i<80;i++)	printf(".");
	printf("\n");

	RTC_config();
	initADC2518();
	
	
//	init_Port_DDAs();
//	 
//  init_Port_Settings(); 
//	init_1024HzInterrupt();
	init_twi();
	initPressureSensors();	
	init_adc();
//	initDisplayPort();
	initPressureSensors();
	ENABLE_INTERRUPTS;
//	RTC_config();
	//printf("%s \n", get_fmtTime());
//	init_RH();   // do after reading settings.	
	printf(".....\n....\n...\n.\n");
}

