/*
 * ADC2518.h
 *
 * Created: 8/14/2023 4:23:35 PM
 *  Author: greg
 */ 


#ifndef ADC2518_H_
#define ADC2518_H_

void initADC2518(void);
void ADC_wrGPO(uint8_t output, uint8_t value);
void ADC_rdSystemStatus(void);
void ADC_OverSample(uint8_t log2Samples);   
void ADC_manualMode(void);
uint16_t ADC_ReadManualModeAnalogCh(uint8_t NextChannel);
void ADC_OscSpeed(uint8_t speed);   



#define ADC2518_HiSpeedOSC 0
#define ADC2518_LowSpeedOSC 1


#endif /* ADC2518_H_ */