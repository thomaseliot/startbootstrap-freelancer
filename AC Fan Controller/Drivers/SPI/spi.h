/*
 * spi.h
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 


#ifndef SPI_H_
#define SPI_H_

#include "cmr_64c1_lib.h"
#include "spi_config.h"

#define SPI_MODE_MASK		0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK		0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK	0x01  // SPI2X = bit 0 on SPSR

#define SPI_LSBFIRST		0
#define SPI_MSBFIRST		1

typedef enum {
	SPI_CLOCK_DIV4  = 0x00,
	SPI_CLOCK_DIV16 = 0x01,
	SPI_CLOCK_DIV64 = 0x02,
	SPI_CLOCK_DIV128= 0x03,
	SPI_CLOCK_DIV2  = 0x04,
	SPI_CLOCK_DIV8  = 0x05,
	SPI_CLOCK_DIV32 = 0x06
} SPI_CLOCK_DIV_t;

typedef enum {
	SPI_MODE0 = 0x00,
	SPI_MODE1 = 0x04,
	SPI_MODE2 = 0x08,
	SPI_MODE3 = 0x0C
} SPI_MODE_t;

// Functions
void spiInit();

void spiSetClockDivider(SPI_CLOCK_DIV_t rate);
void spiSetBitOrder(uint8_t bitOrder);
void spiSetDataMode(SPI_MODE_t mode);

void spiSelect(SPISlave slave);
void spiDeselect(SPISlave slave);
void spiDeselectAll(void);
void spiSwitchSlave(SPISlave slave);

void spiWrite(uint8_t byte);
uint8_t SPI_read(void);
uint8_t SPI_transaction(uint8_t byte, uint8_t read_only);



#endif /* SPI_H_ */