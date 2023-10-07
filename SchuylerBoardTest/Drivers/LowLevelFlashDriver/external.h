/**
* external.h
*
* Copyright (c) 2009-2015 Micron Technology, Inc.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/
#include "types.h"

#define le16_to_cpu(x) x
#define le32_to_cpu(x) x

#define min(a,b) (((a)<(b))?(a):(b))
#define max(a,b) (((a)>(b))?(a):(b))


/* Ability of SPI Controllers */
#define SPI_OPM_RX_QUAD		1 << 3 /* Support 4 pin RX */
#define SPI_OPM_RX_DUAL		1 << 1 /* Support 2 pin RX */
#define SPI_OPM_TX_QUAD		1 << 0 /* Support 4 pin TX */

/* Flags of SPI Transfer */
#define SPI_XFER_DUAL		1 << 30 /* 2 pin data transfer */
#define SPI_XFER_QUAD		1 << 31 /* 4 pin data transfer */
#define SPI_XFER_BEGIN		0x01	/* Assert CS before transfer */
#define SPI_XFER_END		0x02	/* Deassert CS after transfer */


struct spi_slave {
	u8 op_mode_rx;
	u8 op_mode_tx;
	unsigned int max_speed_hz;
	u8 option;
};

int spi_MT_flash_xfer(struct spi_slave *slave, unsigned int bitlen, const void *dout, void *din, unsigned long flags);void *spi_MT_flash_allocate_mem(size_t size);unsigned long long spi_MT_flash_get_ticks(void);void spi_MT_flash_udelay(unsigned int us);
/*
 * MUST IMPLEMENT -- SPI Transfer function
 * @slave: spi device structure
 * @bitlen: transfer length in bit
 * @dout: buffer for TX
 * @din: buffer for RX
 * @flags: flags for SPI transfer
 */
static inline int spi_xfer(struct spi_slave *slave, unsigned int bitlen,
		const void *dout, void *din, unsigned long flags)
{
	return spi_MT_flash_xfer(slave, bitlen, dout, din, flags);
}

/* MUST IMPLEMENT */
static inline void free(void *buf)
{
	return;
}
/* MUST IMPLEMENT */
static inline void *malloc(size_t size)
{
	return spi_MT_flash_allocate_mem(size);
}
/*
static inline void* memcpy(void* dst, const void* src, size_t size)
{
	return spi_MT_flash_copy_mem(dst, src, size);
}
*/
/* MUST IMPLEMENT */
/* no I mustn't */
/*
static inline void* memset(void* buf, int val, size_t size)
{
	return spi_MT_flash_set_mem(buf, val, size);
}
*/

/* MUST IMPLEMENT */
/* no I mustn't */
/*static inline int printf(const char* fmt, ...)
{
	return spi_MT_flash_printf(fmt, ...);
}
*/
/* MUST IMPLEMENT -- get system current tick(unit: 10ms) function */
static inline unsigned long long get_ticks(void)
{
	return spi_MT_flash_get_ticks();
}
/* MUST IMPLEMENT -- microsecend delay function */
static inline void udelay(unsigned int us)
{
	return spi_MT_flash_udelay(us);
}
