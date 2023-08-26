/*
 * spi_RTC.c
 *
 * Created: 7/12/2023 2:37:29 PM
 *  Author: greg
 */ 
#include <avr/io.h>
#include "spi.h"
#include "EnablesAndCSs.h"
/***************************
 *
 *	RTC -- Real Time Clock Defines
 *
*****************************/
#define RTC_WRITE_CMD 0x20   // OR with 5-bit register address
#define RTC_READ_CMD  0xA0   // OR with 5-bit register address
// RTC register address
#define RTC_CTL1	0x00  //
#define RTC_CTL2	0x01  //
#define RTC_CTL3	0x02  //
#define RTC_SEC	    0x03  // seconds and OSF flag in bit 7.
#define RTC_MIN	    0x04
#define RTC_HRS     0x05
#define RTC_DAY     0x06
#define RTC_WKD     0x07  // week day. Sunday is 0.
#define RTC_MTH     0x08
#define RTC_YR      0x09
// Not using alarm registers, timestamp or watchdog registers.
#define RTC_ALSEC     0x0A  // second alarm
#define RTC_ALMIN     0x0B  // minute  alarm
#define RTC_ALHR      0x0C  // hour  alarm
#define RTC_ALDAY     0x0D  // day  alarm
#define RTC_ALWKD     0x0E  // wkDay alarm
#define RTC_CLKOUT    0x0F  // clock out Control
#define RTC_WDTCTL    0x10  // watchdog timer control
#define RTC_WDTVAL    0x11 // watchdog timer value
// 0x12 to 0x18 are time stamp registers. not using define later
#define RTC_AGEOFF    0x19  // aging offset


#define RTC_32768Hz    0x00
#define RTC_16384Hz    0x01
#define RTC_8192Hz     0x02
#define RTC_4096Hz     0x03
#define RTC_2048Hz     0x02
#define RTC_1024Hz     0x05
#define RTC_1Hz        0x06
#define RTC_OSCILLATOR_STOP_FLAG  7  // bit 7 of seconds register.

#define RTC_INT_FREQ     1024              //0.9765625 msec per count, 1024 is 1 second, from RTC chip on pin int 7
#define RTC_BLF		2    // low battery flag bit in Control Register 3
/*************************************   Transfer Bytes to the RTC via the SPI port.*   first byte is the write command and register address*************************************/uint8_t spi_RTC_Write(uint8_t addr, uint8_t *mosi,  uint8_t numBytes){	selectRTC();	spiTransferByte( RTC_WRITE_CMD | addr);	spiSendBytes(mosi, numBytes);	deSelectRTC();	return numBytes;}uint8_t spi_RTC_Read(uint8_t addr, uint8_t *miso,  uint8_t numBytes){	selectRTC();	spiTransferByte(RTC_READ_CMD | addr);	spiGetBytes(miso, numBytes);	deSelectRTC();	return numBytes;}