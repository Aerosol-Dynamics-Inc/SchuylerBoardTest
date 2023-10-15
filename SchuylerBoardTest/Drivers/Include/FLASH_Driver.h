/*
 * FLASH_Driver.h
 *
 * Created: 8/19/2023 2:21:34 PM
 *  Author: greg
 */ 


#ifndef FLASH_DRIVER_H_
#define FLASH_DRIVER_H_

uint8_t spi_FlashInitialize(void);
void spi_FlashReadID(void);
uint8_t spi_FlashWrite(uint32_t address, uint8_t *buffer,  uint8_t numBytes);
uint8_t spi_FlashWriteToPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes, uint8_t oob);uint8_t spi_FlashRead(uint32_t address, uint8_t *buffer,  uint8_t numBytes);uint8_t spi_FlashReadFromPage(uint32_t paddress, uint16_t address, uint8_t* buffer, uint8_t nbytes, uint8_t oob);uint8_t spi_FlashMapAddress(uint32_t address, uint32_t *pageAddress, uint16_t *byteAddress);uint8_t spi_FlashEraseAllBlocks();uint8_t spi_FlashEraseBlock(uint32_t baddress);void spi_FlashUnlockAllBlocks(void);void spi_FlashDisplayFeatureRegisters(void);void spi_FlashDisplayStatusRegister(void);uint8_t spi_FlashReset(void);
#endif /* FLASH_DRIVER_H_ */