/*
 * Ports.h
 *
 * Created: 6/28/2023 4:20:27 PM
 *  Author: greg
 */ 


#ifndef PORTS_H_
#define PORTS_H_

#define  DISPLAY_DATA_LINES_PORT   PORTA

#define  DDRA_SETTING  0xFF    // all Output for data.

// Port B:  FET3 & SPI ; SPI CS also on PH8;
#define SPI_CS_FRAM	 PB0
#define SPI_SCLK	 PB1
#define SPI_MOSI	 PB2
#define SPI_MISO	 PB3      // INPUT
#define SPI_CS0		 PB4
#define SPI_CS1      PB5
#define SPI_CS2		 PB6
#define FET3		 PB7
#define  DDRB_SETTING  (0xFF & ~_BV(SPI_MISO))  // all Output for data.
#define SPI_CS_FRAM_PORT  PORTB
#define SPI_CSx_PORT PORTB


//Port C   Goes to J32 -- use for future or timing outputs
#define  DDRC_SETTING  0xFF    // all Output for data.

//Port D:
#define I2C_SCL PD0
#define I2C_SDA PD1
#define RX1 PD2   // INPUT!!
#define TX1 PD3
#define LASER_SHUTDOWN  PD4
// No Connect PD5
#define PULSE_D1	PD6     //INPUT!!
// No Connect PD7
#define  DDRD_SETTING  (0xFF & ~(_BV(RX1) | _BV(PULSE_D1)) )       

// Port E:
#define RX0                 PE0          //INPUT!!
#define TX0                 PE1
#define WICK_SENSOR_POWER   PE2
#define TP65				PE3
#define TP66				PE4
#define LOW_VOLTAGE_INT		PE5
#define PULSE_D2			PE6     //INPUT!!
#define RTC_1024HZ_INT		PE7     //INPUT!!
#define DDRE_SETTING  ( 0xFF & ~(_BV(RX0) | _BV(PULSE_D2) | _BV(RTC_1024HZ_INT)) )


// Port F:  ALL INPUTS
#define ADC0    PF0     
#define ADC1    PF1
#define ADC2	PF2
#define ADC3	PF3
#define JTAG0	PF4
#define JTAG1	PF5
#define JTAG2	PF6     
#define JTAG3	PF7     
#define DDRF_SETTING 0x00

//Port G:
#define BUCK2_EN		PG0   // Moderator 
#define BUCK3_EN		PG1   // Optics HTr
//#define				PG2   // no connection
#define SD_CARD_DETECT	PG3
//#define				PG4   // no connection
#define FET0			PG5
#define DDRG_SETTING 0xFF


//Port H:
#define SPI_CS_OPTICS_DAC		PH0
#define SPI_CS_CURRENT_ADC		PH1
#define SPI_CS_BUCK_CTRL_DAC	PH2
#define SPI_CS_FLASH			PH3
#define SPI_CS_RTC				PH4
#define SPI_CS_SDCARD			PH5  // ACIVE LOW!!
//#define				PH6   // no connection
#define PULSE_D3			PH7   // INPUT
#define DDRH_SETTING  (0xFF & ~_BV(PULSE_D3))
#define SPI_CS_PRIMARY_PORT PORTH

//Port J:
#define RX3                 PJ0          //INPUT!!
#define TX3                 PJ1
#define MICRO_SWITCH        PJ2          //INPUT!!
#define BUTTON		        PJ3          //INPUT!!
#define DISPLAY_RESET       PJ4       
#define DISPLAY_WR_			PJ5
#define DISPLAY_RD_			PJ6

#define DDRJ_SETTING  (0xFF & ~(_BV(PULSE_D3) | _BV(RX3) | _BV(MICRO_SWITCH) | _BV(BUTTON) ) )
#define DISPLAY_CTL_PORT PORTJ


//		Port K:  -- ALL INPUTS
#define ADC8    PK0
#define ADC9    PK1
#define ADC10	PK2
#define ADC11	PK3
#define ADC12	PK4
#define ADC13	PK5
#define ADC14	PK6
#define ADC15	PK7
#define DDRK_SETTING   0x00


//		Port L:  
#define	HEARTBEAT_LED   PL0
//#define		        PL1 
#define PULSE_D4		PL2
#define	FET1			PL3 
#define FET2			PL4
//#define 				PL5  
#define I2C_MUX_RESET	PL6
#define BUCK5_EN		PL7   // Spare 2 
#define DDRL_SETTING   0xFF

#define I2C_MUX_RESET_PORT	PORTL


#define TOGGLE_HEARTBEAT()	PORTL ^= _BV(HEARTBEAT_LED);



#endif /*PORTS_H_*/


