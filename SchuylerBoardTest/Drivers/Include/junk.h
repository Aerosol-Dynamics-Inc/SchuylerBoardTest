/*
 * junk.h
 *
 * Created: 8/4/2023 2:56:34 PM
 *  Author: greg
 */ 


#ifndef JUNK_H_
#define JUNK_H_


int8_t nextADC(int8_t channel);

void showActiveADCs(void);

int8_t incADC_MUX(int8_t channel);

void init_adc(void);

void doAdc(void);



void startADCcycle(void);




#endif /* JUNK_H_ */