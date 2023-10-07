/*
 * CountersAndTimers.h
 *
 * Created: 7/18/2023 12:21:17 PM
 *  Author: greg
 */ 


#ifndef COUNTERSANDTIMERS_H_
#define COUNTERSANDTIMERS_H_

void init_CountersIimers(void);


//extern volatile uint8_t timer2Flag;
extern volatile uint8_t oneSecFlag, halfSecFlag;
extern volatile uint16_t ms_twiCount;
extern volatile uint16_t counter1024Hz;


#endif /* COUNTERSANDTIMERS_H_ */