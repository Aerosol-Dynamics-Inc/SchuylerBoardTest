/*
 * Globals.h
 *
 * Created: 7/5/2023 5:28:11 PM
 *  Author: greg
 */ 


#ifndef GLOBALS_H_
#define GLOBALS_H_

/**********************************************
 *  Enable and Disable Interrupts.
 ************************************************/

#define DISABLE_INTERRUPTS SREG &= 0x7F
#define ENABLE_INTERRUPTS sei()

/**********************************************
 *  CPU frequency  and  timer2 (for dead time) divider
 ************************************************/
#define CLOCK_14p7456MHZ
#define F_CPU 14745600UL

#define RETURN_OK	0
#define RETURN_ERROR 1


#define DELAY_1us _delay_loop_1(5)					// must have #include <util/delay_basic.h> in any file that uses these.
#define DELAY_20us _delay_loop_1(98)
#define DELAY_40us _delay_loop_1(196)
#define DELAY_100us _delay_loop_2(369)
#define DELAY_120us _delay_loop_2(442)
#define DELAY_140us _delay_loop_2(516)
#define DELAY_540us _delay_loop_2(1991)
#define DELAY_1ms _delay_loop_2(3687)
#define DELAY_2ms _delay_loop_2(3687 * 2)
#define DELAY_5ms _delay_loop_2(3687 * 5)



#endif /* GLOBALS_H_ */