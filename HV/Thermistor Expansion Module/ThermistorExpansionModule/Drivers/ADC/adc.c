/*
 * adc.c
 *
 */ 

#include <avr/io.h>
#include "FreeRTOS.h"
#include "task.h"
#include "adc.h"

/* init_adc
 * Initialize the ADC
 */
void initAdc(void) {
	int i;
	// Initialize channel storage struct
	for(i = THERM1; i <= THERM8; i++) {
		ADCMeta meta = {adcChannelMap[i], 0};
		adcChannels[i] = meta;
	}
	
	// Set configuration registers
	// Set ADC prescaler to 16 - 1MHz sample rate @ 16MHz
	ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0); 
	
	// Left adjust ADC result to allow easy 8 bit reading
	ADMUX  |= (1 << ADLAR);
	// Set ADC high speed mode
	ADCSRB |= (1 << ADHSM);
	
	// No MUX values needed to be changed for initialization
	// Set ADC reference to AREF (AREFEN = 1, ISRCEN = 0, REFS1 = 0, REFS0 = 0)
	ADCSRB |= (1 << AREFEN);
	// Enable ADC
	ADCSRA |= (1 << ADEN);
}

/* adc_read
 * Read an ADC channel, blocking until conversion is complete.
 * Arguments:
 *	ch: the channel number to read
 * Returns: the result of the conversion, 8-bit
 */
uint16_t adcRead(uint8_t ch) {
	// Critical section **********************************************************
	taskENTER_CRITICAL();
	// Set which channel to read. Leave all ADMUX bits as is except MUX4-0
	if(ch > 12) ch = 18;	// Read ground if invalid channel
	ADMUX &= 0xE0;					// Clear MUX bits
	ADMUX |= ch;				// Set MUX to channel
	
	// Set ADC start conversion bit
	ADCSRA |= (1 << ADSC);
	taskEXIT_CRITICAL();
	//****************************************************************************
	
	// Wait for ADCSRA bit ADSC to go low for data conversion to complete
	while(ADCSRA & (1 << ADSC)) {
		// Yield to other tasks if not yet ready
		// Currently commented out because I think this wait time is orders of 
		// magnitude less than the tick time, so it would be best to get all
		// the ADC values as atomically as possible.
		// taskYIELD();
	}
	
	// Return read channel
	return ADCH;
}

/* adc_read_update
 * Read an ADC channel and updates the ADC value struct
 * Arguments:
 *	ch: the channel enumeration ID to read
 * Returns: the result of the conversion, 8-bit
 */
void adcReadUpdate(ADCChannel ch) {
	uint8_t physicalChannel;
	uint16_t channelValue;
	
	// Convert ADCChannel to physical channel
	physicalChannel = adcChannelMap[ch];
	
	// Read channel
	channelValue = adcRead(physicalChannel);
	
	// Update struct
	adcChannels[ch].currentValue = channelValue;
}

/* adc_val
 * Returns the value for the specified ADCChannel
 * Arguments:
 *	ch: the channel enumeration ID to get
 * Returns: the value of that channel 
 */
uint16_t adcVal(ADCChannel ch) {
	return adcChannels[ch].currentValue;
}