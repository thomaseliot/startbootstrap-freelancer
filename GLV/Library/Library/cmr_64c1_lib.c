/*
 * cmr_64c1_lib.c
 * 
 * Library for common functions used with the atmega64c1
 *
 * Copyright (c) Carnegie Mellon Racing 2015
 */

#include "cmr_64c1_lib.h"

/************************************************************************/
/* I/O                                                                  */
/************************************************************************/

/* adc_read
 * Read an ADC channel, blocking until conversion is complete.
 * Arguments:
 *	ch: the channel number to read
 * Returns: the result of the conversion, 8-bit
 */
uint8_t adc_read(uint8_t ch) {
	// Set which channel to read. Leave all ADMUX bits as is except MUX4-0
	// Clear MUX bits
	ADMUX &= 0xE0;
	// Guard against invalid channel
	if(ch > 12) ch = 18; // Read ground
	
	// Set MUX to read channel
	ADMUX |= ch;
	
	// Set ADC start conversion bit
	ADCSRA |= (1 << ADSC);
	
	// Wait for ADCSRA bit ADSC to go low for data conversion to complete
	while(ADCSRA & (1 << ADSC));
	
	// Return read value
	return ADCH;
}

/* config_io_pin
 * Configures an digital I/O pin as input or output.
 * Arguments:
 *	port: the port to configure, IO_PORT_B, C, or D
 *  port_ch: the channel to set, 0-7
 *  dir: the direction, IO_DIR_INPUT or IO_DIR_OUTPUT
 * Returns: void
 */
void config_io_pin(uint8_t port, uint8_t port_ch, uint8_t dir) {
	switch(port){
		case IO_PORTB:
			// Crazy bit trickery that sets specific bit to value
			// without touching other bits
			DDRB ^= ((-dir) ^ DDRB) & (1 << port_ch);
			break;
		case IO_PORTC:
			DDRC ^= ((-dir) ^ DDRC) & (1 << port_ch);
			break;
		case IO_PORTD:
			DDRD ^= ((-dir) ^ DDRD) & (1 << port_ch);
			break;
		default:
			break;
	}
}

/* set_io_pin
 * Sets an digital I/O pin as high or low.
 * Arguments:
 *	port: the port to configure, IO_PORT_B, C, or D
 *  port_ch: the channel to set, 0-7
 *  val: the value, HIGH or LOW
 * Returns: void
 */
void set_io_pin(uint8_t port, uint8_t port_ch, uint8_t val) {
	switch(port){
		case IO_PORTB:
			// Crazy bit trickery that sets specific bit to value
			// without touching other bits
			PORTB ^= ((-val) ^ PORTB) & (1 << port_ch);
			break;
		case IO_PORTC:
			PORTC ^= ((-val) ^ PORTC) & (1 << port_ch);
			break;
		case IO_PORTD:
			PORTD ^= ((-val) ^ PORTD) & (1 << port_ch);
			break;
		default:
			break;
	}
}