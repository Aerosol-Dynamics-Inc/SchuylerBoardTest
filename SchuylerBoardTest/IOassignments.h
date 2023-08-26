/*
 * IO.h
 *
 * Created: 7/5/2023 3:04:35 PM
 *  Author: greg
 */ 


#ifndef IO_H_
#define IO_H_

typedef enum {  in_Adc0, in_Adc1, in_Adc2, in_Adc3,    // adc 4-7 used for JTAG. skip those identifiers for simplicity.
	    in_Adc8 = 8, in_Adc9, in_AdcWick, in_AdcLaserCur, 
		in_Adc12, in_Adc13, in_Adc14, in_Adc15 = 15, in_AdcPSV=15,
		in_RHT, in_DiffPress, in_AbsPress, in_Press3, 
		in_BuckCur0, in_BuckCur1, in_BuckCur2, in_BuckCur3, in_BuckCur4,in_BuckCur5,
		in_microSwitch, in_Button		
	 }  Inputs;
	
typedef enum {  out_Buck0, out_Buck1, out_Buck2, out_Buck3, out_Buck4, out_Buck5, 
	    out_pump,
		out_FET0, out_FET1, out_FET2, out_FET3,
		out_PWM0, out_PWM1, out_PWM2, out_PWM3
	  }  Outputs;






#endif /* IO_H_ */