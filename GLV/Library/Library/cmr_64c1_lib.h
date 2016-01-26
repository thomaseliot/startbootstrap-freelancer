/*
 * cmr_64c1_lib.h
 * 
 * Library for common functions used with the atmega64c1
 *
 * Copyright (c) Carnegie Mellon Racing 2015
 */ 


#ifndef CMR_64C1_LIB_H_
#define CMR_64C1_LIB_H_

#include <stdint.h>
#include <avr/io.h>

/************************************************************************/
/* Constants & Definitions                                              */
/************************************************************************/

// IO port definitions, for use in helper functions
#define IO_PORTB		0
#define IO_PORTC		1
#define IO_PORTD		2
// IO direction definitions, for use in helper functions
#define IO_DIR_INPUT	0
#define IO_DIR_OUTPUT	1
// IO value set definitions, for use in helper functions
#define LOW		0
#define HIGH	1

// Pin definitions for SPI
#define SPI_MISO	0	// PB0
#define SPI_MOSI 1	// PB1
#define SPI_SCK	7	// PB7
#define SPI_SSA	1	// PC1


/************************************************************************/
/* I/O                                                                  */
/************************************************************************/

// Read ADC channel
uint8_t adc_read(uint8_t ch);
// Configure I/O pin
void config_io_pin(uint8_t port, uint8_t port_ch, uint8_t dir);
// Set value of I/O pin
void set_io_pin(uint8_t port, uint8_t port_ch, uint8_t val);

#endif /* CMR_64C1_LIB_H_ */