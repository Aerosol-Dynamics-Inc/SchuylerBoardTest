/*
 * TWI_MUX.h
 *
 * Created: 8/4/2023 4:30:38 PM
 *  Author: greg
 */ 


#ifndef TWI_MUX_H_
#define TWI_MUX_H_


#define MUX_OFF	0				// mux command 0 selects no channels
#define MUX_CH_ABS          1   // 15PSI Absolute     mux command 1 selects SDA0 and SCL0
#define MUX_CH_DIFF_1PSI    2   // 1 PSI Differential  mux command  2 selects SDA1 and SCL1
#define MUX_CH_U47 		    3   // mux command 3 selects SDA2 and SCL2
#define MUX_UNUSED			4   // mux command 4 selects SDA3 and SCL3 -- NOT CONNECTED TO ANYTHING!


uint8_t setI2Cmux(uint8_t whichMUXch);
uint8_t getMuxConfiguration(void);



#endif /* TWI_MUX_H_ */