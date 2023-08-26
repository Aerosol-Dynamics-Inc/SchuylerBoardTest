/**************************************************************************************
 *   File Name: twi_utils.c
 *
 * Description: Control Code writing and reading the TWI  (aka I2C) bus.  
 *				Assumes ATMEGA uC is the one and only master.
 *              No chips on this board require a write pointer so eliminated code for that.
 *				The NPA700 pressure chips have an SCK of 100 to 400 Khz. So using 307.2kHz
 *
 * Updates needed: add code to send restart in case of errors.
 *	 			Change code to use the twi_error() function.
 *				add code to read and write at address pointer (not sure we need this.)
 *				use or eliminate verbose flag.
 *************************************************************************************/
#include <util/twi.h>
#include <avr/pgmspace.h>
#include <util/delay_basic.h>
#include <inttypes.h>
#include <stdio.h>
#include <inttypes.h>
#include <string.h> 
#include "Globals.h"
#include "Ports.h"
#include "CountersAndTimers.h"
#include "twi_utils.h"
//#include "ErrorHandling.h"

/*
 * define some handy TWI constants
 */
#define TWCR_START     ( _BV(TWINT) | _BV(TWSTA)| _BV(TWEN))
#define TWI_MASTER_TX  (_BV(TWINT) | _BV(TWEN))
#define TWI_TIMEOUT   30   // 
#define TWI_ACK       1
#define TWI_NACK      0
#define TWI_VERBOSE   1   // GSL
#define TWI_QUIET     0   // GSL
#define TWI_MAX_ITER  15 // dd was 250


//i2c status codes
#define TWI_START_SENT 0x08
#define TWI_SLA_W_SENT_ACK_RECIEVED  0x18
#define TWI_SLA_W_SENT_ACK_NOT_RECIEVED  0x20
#define TWI_DATA_BYTE_SENT_ACK_RECIEVED  0x28
#define TWI_DATA_BYTE_SENT_ACK_NOT_RECIEVED  0x30
#define TWI_SLA_R_SENT_ACK_RECIEVED  0x40
#define TWI_SLA_R_SENT_ACK_NOT_RECIEVED  0x48


const char s_twi_start_error[]   PROGMEM = "";  //"TWI START CONDITION ERROR";
const char s_twi_sla_w_error[]   PROGMEM = ""; //"TWI SLAVE ADDRESS ERROR";
const char s_twi_data_tx_error[] PROGMEM = ""; //"TWI DATA TX ERROR";
const char s_twi_data_rx_error[] PROGMEM = ""; //"TWI DATA RX ERROR";
const char s_twi_timeout[]       PROGMEM = ""; //"TWI TIMEOUT";
const char s_twi_error[]         PROGMEM = "";  //"TWI ERROR\n";
const char s_fmt_twi_error[]     PROGMEM = "";  //" TWCR=%02x STATUS=%02x\n";

static uint8_t verbose;

inline void twi_stop();
void    twi_error(const char * message, uint8_t cr, uint8_t status);
int8_t twi_start();

#define E_TWI_ERROR "twiError"

void setStatus(char *code)
{
	printf("TWI status code %s\n", code);
	
}

void showERRORcode(uint8_t code) 
{
	printf("TWI error code %X\n", code);
}




/*******************************************************
 *  init_twi()  : reset MUX, set SCK speed and enable twi interface
 * 
 *		SCL frequency =  CPU_FREQ / (16 + 2 * TWBR * 4^TWPS)
 *		Note TWPS are bits 0&1 in TWSR register;
 *			TWPS == 0 --> SCK =	 14.7456 Mhz /(16 + 2 * TWBR) ;    // TWBR = 1 --> 819.4  Khz;  TWBR = 16 -->  307.2 Khz
 *			TWPS == 1 --> SCK =	 14.7456 Mhz /(16 + 8 * TWBR) ;    // TWBR = 1 --> 614.4  Khz;	TWBR = 16 -->  102.4 Khz	
 *			TWPS == 2 --> SCK =	 14.7456 Mhz /(16 + 32 * TWBR) ;   // TWBR = 1 --> 307.2 Khz
 *			TWPS == 2 --> SCK =	 14.7456 Mhz /(16 + 128 * TWBR) ;  // TWBR = 1 --> 102.4 Khz
********************************************************/

void init_twi(void)
{
	//reset i2c mux just in case of problem;
	twi_stop();
	I2C_MUX_RESET_PORT &= ~_BV(I2C_MUX_RESET);  	
	for (uint8_t i=0; i<20; i++) DELAY_5ms;
	I2C_MUX_RESET_PORT |=  _BV(I2C_MUX_RESET);   
	for (uint8_t i=0; i<20; i++) DELAY_5ms;
	I2C_MUX_RESET_PORT &= ~_BV(I2C_MUX_RESET);  	
	for (uint8_t i=0; i<20; i++) DELAY_5ms;
	I2C_MUX_RESET_PORT |=  _BV(I2C_MUX_RESET);   
	
	
	TWSR &= ~0x03;    // clear TWSP bits of TWSR.
	TWSR |=  0x0;     // set TWSP to 0, div by 1 
	TWBR  = 16;       // 16 --> 307.2 Khz for TWSP = 0
	TWCR |= _BV(TWEN);
	
	
/*		uint8_t error=0;
		for (uint8_t i=0; i<255 ;++i){
			if ( (PIND & _BV(I2C_SCL)) || (PIND & _BV(I2C_SDA)) ) {
				error = 1;
				DELAY_1us;
			} else error = 0;
		}
		if (error) printf("WFT?? SCL or SDA not high!!??\n");//
*/
	
	
	verbose = 1;   
}

/*******************************************************
// wait until transmission is complete; return ERROR if timed out
********************************************************/
inline uint8_t waitForTransmit(void)
{
	ms_twiCount = 0; 
	while (!(TWCR & _BV(TWINT)) && (ms_twiCount < TWI_TIMEOUT)) 
	{;} 	
	return (ms_twiCount >= TWI_TIMEOUT);  // inconsistent coding practice here, but works as long as RETURN_ERROR==1
}

/*******************************************************
// assign byte to TWDR then clear interrupt to start byte transmission.  
********************************************************/
inline void	sendByte(uint8_t byte)
{
	TWDR = byte;
	TWCR = _BV(TWINT) | _BV(TWEN); /* clear interrupt to start transmission */
}

/*******************************************************
//  START--signal an TWI start condition in preparation for an TWI bus transfer sequence (polled)
********************************************************/

int8_t twi_start()
{
  uint8_t status;
  
  TWCR = TWCR_START;  //send start to take control of the bus 

  if ( waitForTransmit() ) 
  {
	printf("twi start timeout... \r\n" );
    //twi_error(s_twi_start_error, TWCR, TWSR);
	//twi_error(s_twi_timeout, TWCR, TWSR);
    return RETURN_ERROR;
  }
  /* verify start condition */
  status = TWSR & 0xF8;
  if (status != TWI_START_SENT) {   
	 printf("twi start wrong status %d... \r\n", status );
	 return RETURN_ERROR;
  }
  return RETURN_OK;
}
/*******************************************************
 * STOP--signal the end of an TWI bus transfer 
********************************************************/
inline void twi_stop()
{
  TWCR = _BV(TWINT)|_BV(TWEN)|_BV(TWSTO);
}

/*******************************************************
// display the TWI status and error message and release the TWI bus 
********************************************************/
void twi_error(const char * message, uint8_t cr, uint8_t status)
{
  if (verbose)
  {
  	printf_P(message);
  	printf_P(s_fmt_twi_error, cr, status);
  }
  twi_stop();
}

/*******************************************************
   write numBytes of *dataOut to twi address
	returns either RETURN_OK or RETURN_ERROR
********************************************************/
int twi_write_bytes(uint8_t twi_addr,  uint8_t* dataOut, uint8_t numBytes)
{

	if (twi_start()!=RETURN_OK)  { 	return RETURN_ERROR; 	}

	sendByte( (twi_addr <<1) | TW_WRITE );  // send Address and write command
	waitForTransmit();

	uint8_t status = TWSR & 0xF8;
	if (status != TWI_SLA_W_SENT_ACK_RECIEVED)
	{
		twi_stop(); //TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN); /* send stop condition */
		showERRORcode(TWI_SLA_W_SENT_ACK_NOT_RECIEVED);
		return RETURN_ERROR;
	}

	for (uint8_t i=0;i<numBytes; i++)
	{		
		sendByte(*dataOut++);
		waitForTransmit();
		status = TWSR & 0xF8;
		if (status != TWI_DATA_BYTE_SENT_ACK_RECIEVED )
		{
			printf("byte sent status = =0x%02X\n",status);
			TWCR = _BV(TWINT) | _BV(TWSTO) | _BV(TWEN);  /* send stop condition */
			showERRORcode(TWI_DATA_BYTE_SENT_ACK_NOT_RECIEVED);
			return RETURN_ERROR;
		}
	}
	twi_stop();
	return RETURN_OK;
}

/*****************************************************************
 *
 *  twi_read_bytes:  read 'len' bytes from address 'twi_addr' into '*buf'
 *
 *****************************************************************/

int twi_read_bytes(uint8_t twi_addr, uint8_t *buf, int len)
{
	if ( twi_start() != RETURN_OK )  { 	return RETURN_ERROR; 	}

	sendByte( (twi_addr <<1) | TW_READ );  // send Address and read command
	waitForTransmit();
	uint8_t status = TWSR & 0xF8;
	if (status != TWI_SLA_R_SENT_ACK_RECIEVED)
	{
		showERRORcode(TWI_SLA_R_SENT_ACK_NOT_RECIEVED);
		twi_stop();
		setStatus(E_TWI_ERROR);
		return RETURN_ERROR;
	}


	for (; len > 0; len--)
	{
		ms_twiCount =0;
		TWCR = (len==1) ?  (_BV(TWINT)|_BV(TWEN)) : (_BV(TWINT)|_BV(TWEN)|_BV(TWEA));  // clear interrupt to enable and no ACK on last byte	
		if (waitForTransmit())  
		{
			printf("ERROR read failed with %d bytes left, status=0x%02X\r\n", len, TWSR );
			twi_stop();
			setStatus(E_TWI_ERROR);
			return RETURN_ERROR;
		}
		*buf++ = TWDR;
	}
	twi_stop();
	return RETURN_OK;
}
