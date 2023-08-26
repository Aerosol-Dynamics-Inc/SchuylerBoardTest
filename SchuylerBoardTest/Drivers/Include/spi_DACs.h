/*
 * spi_DACs.h
 *
 * Created: 7/17/2023 2:41:48 PM
 *  Author: greg
 */ 


#ifndef SPI_DACS_H_
#define SPI_DACS_H_


void initDACs(void);
void spi_buckCtrlDac(uint8_t channel, uint16_t value);void spi_OpticsDAC(uint8_t channel, uint16_t value);
void spi_OpticsDAC_BroadCast(uint16_t value);void spi_OpticsBuckCtl_BroadCast(uint16_t value);void BuckWTM(void);

#endif /* SPI_DACS_H_ */