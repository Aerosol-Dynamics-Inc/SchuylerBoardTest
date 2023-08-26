/******************************************************************************
*   File Name: uart.c
*  
* Description: Controls UART functions. 
*******************************************************************************
*
* -----------------------------------------------------------------------------
* "THE BEER-WARE LICENSE" (Revision 42):
* <joerg@FreeBSD.ORG> wrote this file.  As long as you retain this notice you
* can do whatever you want with this stuff. If we meet some day, and you think
* this stuff is worth it, you can buy me a beer in return.        Joerg Wunsch
* -----------------------------------------------------------------------------
*
* Stdio demo, UART implementation
*
* $Id: uart.c,v 1.1 2005/12/28 21:38:59 joerg_wunsch Exp $
******************************************************************************/
#include <avr/interrupt.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "Globals.h"
#include "InstrumentAndDataSettings.h"
#include "Ports.h"
#include "uart.h"

/************************************
STUFF FOR UART1
*************************************/


#define SI()       uint8_t cpu_sreg
#define SAVEI()    cpu_sreg = SREG; cli()
#define RESTOREI() if (cpu_sreg & 0x80) sei()

#define RING_BUFSIZE 128
char buf[RX_BUFSIZE];  // for USART0  32 bytes
uint8_t buf1[RING_BUFSIZE];  // for USART1  128 bytes  ring buffer;

/* ring Buffer typedef   */
typedef volatile struct ringbuf_t {
	uint8_t    buflen;
	uint8_t    bufcnt;
	uint8_t   *in;
	uint8_t   *out;
	uint8_t   *buf;
} RINGBUF;

RINGBUF  rb;

uint8_t commandCompleteFlag1;


static void ringbuf_init(uint8_t *buffy, uint8_t buflen);
static void    ringbuf_put  (uint8_t c);
static uint8_t ringbuf_get  ();

/************************************
STUFF FOR UART1
*************************************/




#define RX_BUFFSIZE    16
#define BAUD_RR(f,b)    ((f % (16L*b) >= (16L*b)/2) ? (f / (16L*b)) : (f / (16L*b)) - 1)

char buf[RX_BUFSIZE];  // for USART0
volatile uint8_t commandCompleteFlag0 = 0; // commandCompleteFlag1 = 0;   
volatile int  clrError=0  ;
volatile uint8_t  bufIndex0 = 0;
//volatile uint8_t  bufIndex1 = 0;
volatile uint8_t  commandSize = 0;
volatile uint8_t  outputCharacters = 0;
uint8_t pauseOutputForCmdCnt = 0;
uint8_t currentUart=0;

uint8_t getBaudRate();




// global variables
int8_t init_uart(uint8_t whichUart)
{
	if ( (whichUart==2) || (whichUart>3)) {
		return RETURN_ERROR;
	}
	
	currentUart = whichUart;
	
  uint16_t ubrr_16t;
  /* compute baud rate register value */  
  switch (userSettings.baud) {
	  case BAUD_9600:
		ubrr_16t = BAUD_RR(F_CPU,9600);
		break;
	  case BAUD_460k:
		ubrr_16t = BAUD_RR(F_CPU,460800);
		break;
	  case BAUD_115k:
	  default:
		ubrr_16t = BAUD_RR(F_CPU,115200);
		break;
  }
  switch (whichUart) {
	  case 1:
		UBRR1H = (ubrr_16t >> 8) & 0xff;   // set baud rate.
		UBRR1L = ubrr_16t & 0xff;
		UCSR1B = _BV(TXEN1) | _BV(RXEN1) | _BV(RXCIE1) ; // enable transmitter and Receive  & receive complete interrupt
		ringbuf_init(buf1,RING_BUFSIZE);			
		break;		
	  case 3:
		  UBRR3H = (ubrr_16t >> 8) & 0xff;   // set baud rate.
		  UBRR3L = ubrr_16t & 0xff;
		  UCSR3B = _BV(TXEN3) | _BV(RXEN3) | _BV(RXCIE3) ; // enable transmitter and Receive  & receive complete interrupt
		break;
	  case 0:
	  default:
		  UBRR0H = (ubrr_16t >> 8) & 0xff;   // set baud rate.
		  UBRR0L = ubrr_16t & 0xff;
		  UCSR0B = _BV(TXEN0) | _BV(RXEN0) | _BV(RXCIE0) ; // enable transmitter and Receive  & receive complete interrupt		  
	  break;			
  }
  
  return RETURN_OK;
}



/*
 * Send character c down the UART Tx, wait until tx holding register
 * is empty.
 */
int uart0_putchar(char c, FILE *stream)  
{
  if (c == '\n')  uart0_putchar('\r', stdout);
  loop_until_bit_is_set(UCSR0A, UDRE0);
  UDR0 = c;
  return 0;
}

int uart1_putchar(char c, FILE *stream)     
{
	if (c == '\n') uart1_putchar('\r', stdout);
	loop_until_bit_is_set(UCSR1A, UDRE1);
	UDR1 = c;
	return 0;
	
}

int uart1_put(char c)
{
	if (c == '\n') uart1_put('\r');
	loop_until_bit_is_set(UCSR1A, UDRE1);
	UDR1 = c;
	return 0;
	
}



int uart3_putchar(char c, FILE *stream)  
{
	if (c == '\n') 	uart3_putchar('\r', stdout);
	loop_until_bit_is_set(UCSR3A, UDRE3);
	UDR3 = c;
	return 0;
}

int uarts_getchar(FILE *stream)    // don't use getchar() so have dummy function
{
	return 0;  
}


/******************************************************************************
*                         UART 0 RX INTERRUPT HANDLER                         *
*******************************************************************************
* Description: USART interrupt routine- process character!
*              NOT SURE IF WE NEED TO CHECK FOR UART ERROR ELSEWHERE Don't 
*              think we do but I do anyway.
*      Return: -1, or -2 on error
*              returns 1 if got character, returns 2 is we lost a character.
******************************************************************************/
ISR(USART0_RX_vect)
{
	if (UCSR0A & _BV(FE0) ||  UCSR0A & _BV(DOR0) || commandCompleteFlag0) {
		clrError = UDR0; // clear error	or if comandCompleteFlag ignore character
	}
    else if (bufIndex0 > RX_BUFSIZE -2)  {  // buffer full.  Drop command. 
		clrError  = UDR0;
		bufIndex0 = 0; //RX_BUFSIZE;
		buf[RX_BUFSIZE -1] = 0;
		commandCompleteFlag0 = 0;
		commandSize = 0;			
//		uart0_putchar('\r',stdout);
	}
	else  {
		buf[bufIndex0] = UDR0;
	    if ((buf[bufIndex0] == '\n') || (buf[bufIndex0] == '\r')) {
			commandCompleteFlag0 = 1;  
			++commandSize; 
			buf[bufIndex0] = 0;
			bufIndex0 = 0; 
	    }
		else if (buf[bufIndex0] == 0x08 || buf[bufIndex0] == 0x7F )   {  // backspace or delete
			if (bufIndex0>0) {
				uart0_putchar(0x08,stdout);
				uart0_putchar(' ',stdout);
				uart0_putchar(0x08,stdout);
				bufIndex0--;
				--commandSize; 
				if (outputCharacters)  --outputCharacters; 
			}
		}
		
//		else if (buf[bufIndex0] == 0x08 || buf[bufIndex0] == 0x7F       // backspace or delete
//		       || (( buf[bufIndex0] > '+' ) && (buf[bufIndex0] < '{')) ) {  // accept ',' thru 'z',  but ignores spaces and the following: ! " # $ % & '( ) 
		else if (( buf[bufIndex0] > '+' ) && (buf[bufIndex0] < '{'))  {  // accept ',' thru 'z',  but ignores spaces and the following: ! " # $ % & '( ) 
			bufIndex0++;
			++commandSize;
		}
		else if (buf[bufIndex0] == 0x1B)  // 'esc' -- kill buffer
		{
			uart0_putchar('\r',stdout);
			bufIndex0 = 0;
			commandSize = 0;	
		}
	}
}

static void ringbuf_init(uint8_t *buffy, uint8_t buflen)
{
	rb.buflen = buflen;
	rb.bufcnt = 0;
	rb.in     = buffy;
	rb.out    = buffy;
	rb.buf    = buffy;
}

static uint8_t ringbuf_get()
{
	uint8_t c;
	SI();

	SAVEI();
	if (rb.bufcnt == 0) {
		RESTOREI();
		return 0;
	}

	c = *rb.out++;
	rb.bufcnt--;

	if (rb.out >= rb.buf + rb.buflen) {
		/* wrap around to beginning */
		rb.out = rb.buf;
	}
	RESTOREI();
	return c;
}

static void ringbuf_put(uint8_t c)
{
	SI();

	SAVEI();
	
	if (rb.bufcnt >= rb.buflen) {
		RESTOREI();
		return;
	}

	*rb.in++ = c;
	rb.bufcnt++;

	if (rb.in >= rb.buf + rb.buflen) {
		/* wrap around to beginning */
		rb.in = rb.buf;
	}
	RESTOREI();
}


/******************************************************************************
*                         UART 0 RX INTERRUPT HANDLER                         *
*******************************************************************************
* Description: USART interrupt routine- process character!
*              NOT SURE IF WE NEED TO CHECK FOR UART ERROR ELSEWHERE Don't
*              think we do but I do anyway.
*      Return: -1, or -2 on error
*              returns 1 if got character, returns 2 is we lost a character.
******************************************************************************/
ISR(USART1_RX_vect)
{
	
	if (UCSR1A & _BV(FE1) ||  UCSR1A & _BV(DOR1) ) {
		clrError = UDR1; // clear error	or if comandCompleteFlag ignore character
	}
	else  {
		char c = UDR1;
		if ((c == '\n') ) {
			ringbuf_put('\0');  // replace line return with end of string.
			commandCompleteFlag1++;
		}
		else if ((c == '\r')) {   // ignore
			//ringbuf_put('\n');
			;
		}

		else if (c == 0x08 || c == 0x7F )   {  // backspace or delete
			c = ringbuf_get();
		}
		else if ( ( c > 0x1F ) && (c < 0x7F )  ) {  // accept printable characters
			ringbuf_put(c);
		}
		//		else if (c == 0x1B)  { // 'esc' -- kill buffer
		//			bufIndex1 = 0;
		//		}
	}
}


	
void doUart(void)
{
	if (commandSize) {
		if (outputCharacters<commandSize) {
			if (!commandCompleteFlag0) uart0_putchar(buf[outputCharacters],stdout);
			++outputCharacters;
			pauseOutputForCmdCnt = 10;			// decremented once per second
		} else if (!pauseOutputForCmdCnt) {   // timed out
			DISABLE_INTERRUPTS;
			outputCharacters = 0;
			commandSize = 0;
			bufIndex0 = 0;		
			ENABLE_INTERRUPTS;		
		}			
	} else {
		outputCharacters = 0; 
		pauseOutputForCmdCnt = 0;		// needed if esc is pressed or if all characters were deleted.
	}
	
	if (commandCompleteFlag0) 	{
//		uart0_putchar(':',stdout);
//		uart0_putchar(' ',stdout);
//		parseCommand(buf);
		printf("    : %s\n",buf);
		outputCharacters = commandSize = pauseOutputForCmdCnt = bufIndex0 = 0;		
	}	
	
	if (commandCompleteFlag1) {
		printf("got Uart1 COmplete:%s \n", buf1);
		uart1_put('~');
		uart1_put('*');
		uart1_put('~');		
	}
}


uint8_t parseBaudRate(char **argv, uint8_t argc)
{
	uint8_t ret = RETURN_OK;
	
	if (!strcmp(argv[0],"b?")) 	{
		printf_P(PSTR("\tbaud,9600 -- set start up baud to 9600\n"
		"\t baud,115k -- set start up baud to 115,200\n"
		"\t baud,460k -- set start up baud to 460,800\n"
		"\t buad must be set to 115k for firmware updates\n"
		"\t NOTE: Changes take effect in 2 seconds but must be saved to eeprom (type \"weeprom\") to survive reset/power cycle\n"));
	}
	else if ( !strcmp(argv[0], "baud") )   	{
		if (argc==2) {
			if (strcmp(argv[1],"9600") == 0)	{
				userSettings.baud = BAUD_9600;
			}
			else if (strcmp(argv[1],"115k") == 0)	{
				userSettings.baud = BAUD_115k;
			}
			else if (strcmp(argv[1],"460k") == 0)	{
				userSettings.baud = BAUD_460k;
			}
			else { ret = RETURN_ERROR; }
			init_uart(currentUart);
		}
		else if (argc==1) 	{
			if (userSettings.baud == BAUD_9600)			{
				printf_P(PSTR("9600\n"));
			}
			else if (userSettings.baud == BAUD_115k)		{
				printf_P(PSTR("115200  \n"));
			}
			else if (userSettings.baud == BAUD_460k)		{
				printf_P(PSTR("460800  \n"));
			}
			else				{
				printf_P(PSTR("ERROR in baud Rate Setting!!!\n"));
			}
		}
	}
	else
		ret = RETURN_ERROR;
	
	return ret;
}



