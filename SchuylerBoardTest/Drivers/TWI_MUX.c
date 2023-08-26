/*
 * TWI_MUX.c  -- PCA9546AC,118    i2c switch
 *
 *	RH sensor does not use MUX  
 *
 * Created: 8/4/2023 3:46:54 PM
 *  Author: greg
 */ 




/*
 * I2CMUX_PCA9546.c
 *
 * Created: 5/8/2018 11:24:45 AM
 *  Author: eljeffe
 */ 

#include <stdio.h>
#include "Globals.h"
#include "TWI_MUX.h"
#include "twi_utils.h"
//#include "ErrorHandling.h"


#define I2C_PCA9546_MUX_ADDR    0x70

//#define MUX_TEMPERATURE_HUMID        // no need select address




/*******************************************************************************
*                     SET I2C MUX CONFIGURATION                                *
********************************************************************************
* Description: writes to I2C MUX to choose channel to be selected
*              Note: multiple channels can not be enabled, (code to allow commented out)
*
*      Global: None
*
*   Arguments: whichMaes
*
*      Return: active muxchannel(s)
*******************************************************************************/
uint8_t setI2Cmux(uint8_t whichMUXch)
{
//	uint8_t bytesWritten=0;
	uint8_t ret_code = RETURN_OK;
	static  uint8_t numErrors[4] = {0,0,0,0};
	if ( whichMUXch<4 ) // one and only one channel selected?  0 is none selected; NOTE (x=whichMUXch-1) selects SDAx/SLAx 
	{
			printf ("whichMUXch = %d \n", whichMUXch);
		if (twi_write_bytes(I2C_PCA9546_MUX_ADDR, &whichMUXch, 1))
		{
			if (numErrors[whichMUXch] > 6) {
//				flagError(I2C_LATCH);  // cleared only on reset
				init_twi();
				printf(" el ");
			}
			else
				printf("ERROR - Setting MUX Channel = %d\n", whichMUXch);
			ret_code = RETURN_ERROR;
		}
		else if (getMuxConfiguration() != whichMUXch )
		{
			printf("ERROR - failed to confirm MUX channel.\r\n");
			if (numErrors[whichMUXch] > 6) {
//				flagError(I2C_LATCH);  // cleared only on reset
				init_twi();
				printf(" em ");
			}
			ret_code = RETURN_ERROR;
		}
	}
	else
	{
		printf("ERROR - invalid MUX Channel = %d\r\n", whichMUXch);
		ret_code = RETURN_ERROR;

	}
	return ret_code;
}



/*******************************************************************************
*                             GET MUX CONFIGURATION                            *
********************************************************************************
* Description: 
*              
*      Global: None
*
*   Arguments: None
*
*      Return: active mux channel(s)
*******************************************************************************/
uint8_t getMuxConfiguration(void)
{
	uint8_t channel;
	if (twi_read_bytes(I2C_PCA9546_MUX_ADDR, &channel, 1))
	{
		printf("error reading mux channel\n");
	}
	return channel;
}

