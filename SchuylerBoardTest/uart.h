#ifndef __uart_h__
#define __uart_h__


#define RX_BUFSIZE 32

extern char buf[];  // defined in uart.c
extern uint8_t pauseOutputForCmdCnt;

void doUart(void);
int8_t init_uart(uint8_t whichUart);
int uart0_putchar(char c, FILE *stream);
int uart1_putchar(char c, FILE *stream);
int uart3_putchar(char c, FILE *stream);
int uarts_getchar(FILE *stream);  // dummy function


uint8_t parseBaudRate(char **argv, uint8_t argc);
extern volatile uint8_t  commandCompleteFlag0;


extern uint8_t pauseOutputForCmdCnt;
// extern volatile uint8_t uart_c;
// extern volatile int uart_state;

#endif
