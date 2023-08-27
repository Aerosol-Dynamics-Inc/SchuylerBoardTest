/*
 * FLASH_Driver.c
 *
 * Created: 7/13/2023 5:02:42 PM
 *  Author: greg
 */ 

#include <avr/io.h>
#define FLASH_WRITE_DISABLE             0x04// 0,0,0
#define FLASH_BLOCK_ERASE               0xD8// 3,0,0
#define FLASH_PROGRAM_EXECUTE           0x10// 3,0,0   Array program
#define FLASH_LOADx1                    0x02// 2,0,1-2176
#define FLASH_LOADx4                    0x32// 2,0,1-2176  CAN'T USE
#define FLASH_LOAD_RANDOM_DATAx1        0x84// 2,0,1-2176
#define FLASH_LOAD_RANDOM_DATAx4        0x34// 2,0,1-2176   CAN'T USE
#define FLASH_PERMANENT_BLOCK_LOCK_PROTECTION 0x2C//3,0,0 

// register addresses for feature bits (status)
#define FLASH_FEATURE_REG_BLOCK_LOCK	0xA0
#define FLASH_FEATURE_REG_CONFIG		0xB0
#define FLASH_FEATURE_REG_STATUS		0xC0
#define FLASH_FEATURE_REG_DIE_SELECT	0xD0

// bit masks for feature status register
#define FLASH_STATUS_BIT_OIP			0x01
#define FLASH_STATUS_BIT_WEL			0x02
#define FLASH_STATUS_BIT_EFAIL			0x04
#define FLASH_STATUS_BIT_PFAIL			0x08
#define FLASH_STATUS_BIT_ECCS0			0x10
#define FLASH_STATUS_BIT_ECCS1			0x20
#define FLASH_STATUS_BIT_ECCS2			0x30
#define FLASH_STATUS_BIT_CRSBY			0x40

	{
		retry--;
	} while (spi_FlashIsBusy() && retry);