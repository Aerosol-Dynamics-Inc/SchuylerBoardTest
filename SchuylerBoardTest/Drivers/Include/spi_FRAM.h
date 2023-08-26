/*
 * spi_FRAM.h
 *
 * Created: 7/21/2023 4:44:29 PM
 *  Author: greg
 */ 


#ifndef SPI_FRAM_H_
#define SPI_FRAM_H_


uint8_t FRAM_ReadStatusRegister(void);
void read_FRAM_DeviceID(void);
void wrFRAM(void);
void framReadSerialNumber(void);void framSaveSerialNumber(uint16_t SN);
void spi_FRAM_writeEnable(void);




#endif /* SPI_FRAM_H_ */