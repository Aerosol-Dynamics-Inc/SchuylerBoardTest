/*
 * spi_RTC.h
 *
 * Created: 7/19/2023 3:48:10 PM
 *  Author: greg
 */ 


#ifndef SPI_RTC_H_
#define SPI_RTC_H_

uint8_t spi_RTC_Read(uint8_t addr, uint8_t *miso,  uint8_t numBytes);
uint8_t spi_RTC_Write(uint8_t addr, uint8_t *mosi,  uint8_t numBytes);


#endif /* SPI_RTC_H_ */