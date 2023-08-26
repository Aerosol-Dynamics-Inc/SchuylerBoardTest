// MODIFIED BY GSL

#ifndef __twi_h__
#define __twi_h__



void init_twi(void);
int  twi_write_bytes(uint8_t twi_addr,  uint8_t* dataOut, uint8_t numBytes);
int	 twi_read_bytes(uint8_t twi_addr, uint8_t *buf, int len);


#endif
