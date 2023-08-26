/*
 * ADC_uC.h
 *
 * Created: 7/6/2023 10:56:27 AM
 *  Author: greg
 */ 


#ifndef ADCSuC_H_
#define ADCSuC_H_

// currently averaging 4 ADC readings.  Check noise on board to see if averaging is the necessary or the right amount.
// ADC_SUM_SHIFT <= 6;  10 bit ADC, leaves six bit for sum.
#define ADC_SUM_SHIFT 0                       
#define NUM_ADC_SUMS (1<<ADC_SUM_SHIFT)       

void init_adc(void);
void doAdc(void);
void startADCcycle(void);
int8_t nextADC(int8_t current);  // remove after testing adc done.
void showActiveADCs(void);

extern int16_t wadc;   // for debugging, saved for 'wadc' command
extern uint8_t  adcCompleteFlag;



#endif /* ADCSuC_H_ */