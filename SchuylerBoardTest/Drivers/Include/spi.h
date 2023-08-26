/*
 * spi.h
 *
 * Created: 4/7/2018 11:11:47 AM
 *  Author: eljeffe
 */ 
#ifndef SPI_H_
#define SPI_H_


void init_spi(void);	
void spiSendBytes(uint8_t *mosi, uint16_t numBytes);void spiGetBytes(uint8_t *miso, uint16_t numBytes);



void spiSend2ByteBigEndian(uint16_t address);
void spiSend4ByteBigEndian(uint32_t address);
uint8_t spiTransferByte(uint8_t mosi);


#endif /* SPI_H_ */