/*
 * spi.c
 * 
 * SPI driver 
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include "spi.h"
#include "FreeRTOS.h"
#include "task.h"

/* spiInit
 * Initializes SPI on the MCU
 */
void spiInit(void) {
	// Function variables
	uint8_t i;
	
	// Set digital pin directions for MISO/MOSI/SCK
	pinMode(SPI_MISO_PORT, SPI_MISO_PIN, IO_DIR_INPUT);
	pinMode(SPI_MOSI_PORT, SPI_MOSI_PIN, IO_DIR_OUTPUT);
	pinMode(SPI_SCK_PORT, SPI_SCK_PIN, IO_DIR_OUTPUT); 
	
	// Set digital pin direction for default SS
	// Note that this must be output, or SPI could go into slave mode
	pinMode(SPI_SS_PORT, SPI_SS_PIN, IO_DIR_OUTPUT);
	
	// Set digital pin directions for all slaves. If they are connected
	// to the default SS, this will just be redundant.
	for(i = 0; i < NUM_SLAVES; i++) {
		pinMode(spiSlaves[i].port, spiSlaves[i].pin, IO_DIR_OUTPUT);
	}
	
	// Deselect all slaves
	spiDeselectAll();
	
	// Enable SPI and master mode
	SPCR = (1 << SPE) | (1 << MSTR); 
	
	// Read SPSR and SPDR to clear them
	i = SPSR;
	i = SPDR;
}

/* spiSetClockDivider
 * Set the SPI clock divider to the specified rate
 * 
 * @param rate the rate to set to
 */
void spiSetClockDivider(SPI_CLOCK_DIV_t rate) {
	SPCR = (SPCR & ~SPI_CLOCK_MASK) | ((uint8_t)rate & SPI_CLOCK_MASK);
	SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | (((uint8_t)rate >> 2) & SPI_2XCLOCK_MASK);
}

/* spiSetBitOrder
 * Set the specified bit order
 * 
 * @param bitOrder the bit order to set to
 */
void spiSetBitOrder(uint8_t bitOrder) {
	if(bitOrder == SPI_LSBFIRST) {
		SPCR |= _BV(DORD);
	} else {	// MSB first
		SPCR &= ~(_BV(DORD));
	}
}

/* spiSetDataMode
 * Set the data mode to the specified mode
 * 
 * @param mode the mode to set to
 */
void spiSetDataMode(SPI_MODE_t mode) {
	SPCR = (SPCR & ~SPI_MODE_MASK) | (uint8_t) mode;
}

/* spiSelect
 * Select the specified slave
 * 
 * @param slave the slave to select
 */
void spiSelect(SPISlave slave) {
	// Set pin low
	setPin(slave.port, slave.pin, LOW);
}

/* spiDeselect
 * Deselect the specified slave
 * 
 * @param slave the slave to select
 */
void spiDeselect(SPISlave slave) {
	// Set pin high
	setPin(slave.port, slave.pin, HIGH);
}

/* spiDeselectAll
 * Deselect all slaves
 */
void spiDeselectAll(void) {
	// Function variables
	uint8_t i;
	
	taskENTER_CRITICAL();
	// Loop through each slave and deselect it
	for(i = 0; i < NUM_SLAVES; i++) {
		spiDeselect(spiSlaves[i]);
	}
	taskEXIT_CRITICAL();
}

/* spiSwitchSlave
 * Deselect all slaves, then select the specified slave
 * 
 * @param slave the slave to select
 */
void spiSwitchSlave(SPISlave slave) {
	// Deselect all
	spiDeselectAll();
	// Select this slave
	spiSelect(slave);
}

/* stpiTransaction
 * Writes a byte over SPI and receives a byte
 * 
 * @param byte byte to send over SPI
 * @readOnly - if set to 1, the data register will not be touched before the transaction begins. 
 * @return received byte
 */
uint8_t spiTransaction(uint8_t byte, uint8_t readOnly) {
	// Make sure SPI module has been enabled
	if( !(SPCR & _BV(SPE)) ) return 0;
	
	// Set data register
	if (!readOnly) {
		SPDR = byte;	
	}
	
	// Wait for transfer to complete
	while (!(SPSR & (1 << SPIF)));
	
	// Return received value
	return SPDR;
}

/* spiRead
 * Reads one byte from SPI. Does not touch the SPI data register before transaction. 
 * 
 * @return received byte
 */
uint8_t spiRead(void) {
	return spiTransaction(0, 1);
}

/* spiWrite
 * Writes a byte over SPI and ignores return value
 * 
 * @param byte byte to send over SPI
 */
void spiWrite(uint8_t byte) {
	// Make sure SPI module has been enabled
	if( !(SPCR & _BV(SPE)) ) return;
	
	// Set data register
	SPDR = byte;
	
	// Wait for transfer to complete
	while (!(SPSR & (1 << SPIF)));
}