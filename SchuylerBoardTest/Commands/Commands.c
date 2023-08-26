/*
 * Commands.c
 *
 * Created: 7/6/2023 11:08:34 AM
 *  Author: greg
 */ 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <ctype.h>
#include <stdio.h>
#include "InstrumentAndDataSettings.h"

uint8_t enableADC(uint8_t argc,void** argv);
uint8_t disableADC(uint8_t argc,void** argv);

typedef enum {arg_null, arg_uint, arg_int, arg_int32, arg_uint32, arg_tenths }  argTypes ;
typedef enum {dsp_null, dsp_uint, dsp_int, dsp_int32, dsp_uint32, dsp_tenths}  displayTypes ;	
	
typedef uint8_t (*CommandHandler)(uint8_t argc,void** argv);


typedef struct
{
	const char* commandString;
	const CommandHandler command;
	const argTypes		arg_type;			// type of argument
	const uint8_t		max_argc;          // maximum number of arguments in command
	const int16_t		min_arg;           // min arg[1] value  
	const int16_t		max_arg;			// max arg[1] value
	const *void			disParam ;			// pointer to display parameter argc == 1
	const displayTypes  disType;       // type of parameter if disParam != NULL   
} CommandTableEntry;	
	

const CommandTableEntry PROGMEM  CommandTable[] = {
//     "cmd_str",  function, argument type, Max argc, min arg, max arg,  dispParam, disType 
	{ "enadc",  enableADC,   arg_uint,		1,			0,		15,		NULL ,	dsp_null},
	{ "disadc", disableADC,  arg_uint,		1,			0,		15,		NULL ,	dsp_null}
//	{ "wadc",   show_u,      arg_null,		0,			0,		0,		&wadc,	dsp_uint}
		
};
		
uint8_t enableADC(uint8_t argc,void** argv) 
{
	hdwConfig.activeADCs |= (1<<argv[1]);
	return 0;
}

uint8_t disableADC(uint8_t argc,void** argv)
{
	hdwConfig.activeADCs &= ~(1<<argv[1]);
	return 0;	
}
		
void show_u(uint8_t argc,void** argv) {
	printf_P(PSTR("%u"), );
}
