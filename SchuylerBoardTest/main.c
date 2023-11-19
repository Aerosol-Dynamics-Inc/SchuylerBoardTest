/*
 * SchuylerBoardTest.c
 *
 * Created: 6/28/2023 11:13:39 AM
 * Author : greg
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay_basic.h>
#include "ADC_uC.h"
#include "CountersAndTimers.h"
#include "Globals.h"
#include "Initialize.h"
#include "EnablesAndCSs.h"
#include "InstrumentAndDataSettings.h"
#include "Ports.h"
#include "testCommands1.h"
#include "uart.h"

parameters userSettings;
measured readings;
configuration  hdwConfig;

int main(void)
{
	
	initialize();
	printf("initialized\n");		

	
    while (1) 
    {
		doUart();    
		if   (commandCompleteFlag0)  {


			processCommand();
			commandCompleteFlag0 = 0;   // processed complete command, can now process more characters. Needs to be after previous line, or new keys could be accepted before reseting those variables.
		}
		
		if (adcCompleteFlag){
//			doAdc();
		}
		
		if (halfSecFlag) {
			TOGGLE_HEARTBEAT();			
			halfSecFlag = 0;
		//	selectRTC();
		}
		
		if (oneSecFlag) {
			TOGGLE_HEARTBEAT();			
			if (pauseOutputForCmdCnt) --pauseOutputForCmdCnt;	
			oneSecFlag = 0;					
		}
		
	}
}
