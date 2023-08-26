/*
 * spi_ADC2518.h
 *
 * Created: 7/27/2023 5:20:42 PM
 *  Author: greg
 */ 


#ifndef SPI_ADC2518_H_
#define SPI_ADC2518_H_

//  ADC Registers
#define SYSTEM_STATUS   0x00
#define GENERAL_CFG		0x01
#define DATA_CFG		0x02
#define OSR_CFG			0x03
#define OPMODE_CFG		0x04
#define PIN_CFG			0x05
#define GPIO_CFG		0x07
#define GPIO_DRIVE_CFG  0x09
#define GPO_VALUE		0x0B
#define GPI_VALUE		0x0D
#define SEQUENCE_CFG	0x10
#define CHANNEL_SEL		0x11
#define AUTO_SEQ_CH_SEL 0x12

//  Set output 6 to GPIO digital output , push/pull
#define PIN_CFG_SETTING         0x40 //  set channel 6 to digital, (others to analog)
#define GPIO_CFG_SETTING        0x40 //  set channel 6 digital to output
#define GPIO_DRIVE_CFG_SETTING  0x40 // set channel 6 output to push/pull
// read/write OPCODES
#define NO_OP   0x00   //
#define REG_RD  0x10   // single register read
#define REG_WR  0x08   // single register write
#define SET_BIT	0x18   // set bit
#define CLR_BIT	0x20   // clear bit
//functional Modes
#define MODE_MANUAL			// 0x00
#define MODE_ON_THE_FLY		// 0x10
#define MODE_AUTO_SEQUENCE  // 0x01



#define OSC_SEL 4 // bit in OPMODE_CFG register


#define DUMMY_DATA 0x00


uint8_t ADC_RegisterRead(uint8_t addr,  uint8_t data);
void ADC_RegisterWrite(uint8_t addr,  uint8_t data);
uint16_t ADC_RegisterReadWrite(uint8_t addr,  uint8_t data);



#endif /* SPI_ADC2518_H_ */