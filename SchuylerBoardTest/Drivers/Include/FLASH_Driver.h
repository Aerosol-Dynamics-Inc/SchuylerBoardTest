/*
 * FLASH_Driver.h
 *
 * Created: 8/19/2023 2:21:34 PM
 *  Author: greg
 */ 


#ifndef FLASH_DRIVER_H_
#define FLASH_DRIVER_H_

void spi_FlashReadID(void);
uint8_t spi_FlashWrite(uint32_t address, uint8_t *buffer,  uint8_t numBytes);
uint8_t spi_FlashRead(uint32_t address, uint8_t *buffer,  uint8_t numBytes);uint8_t spi_FlashEraseAllBlocks();
#endif /* FLASH_DRIVER_H_ */