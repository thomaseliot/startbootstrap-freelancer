/*
 * adc_config.h
 * 
 * Analog to digital converter configuration
 *
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 


#ifndef ADC_CONFIG_H_
#define ADC_CONFIG_H_

// Number of ADC channels to read
#define NUM_ADC_CH			8

// Channel enumeration
// Change these for this node
typedef enum ADCChannels {
	THERM1, THERM2, THERM3, THERM4,
	THERM5, THERM6, THERM7, THERM8
} ADCChannel;

// Lowest and highest channels, used for loops
// Change these for this node
#define LOWEST_ADC_CH		THERM1
#define HIGHEST_ADC_CH		THERM8

// Channel mapping, symbol to physical ADC channel
// in order of the above enumeration
// TODO: Putting static variables in header files is apparently bad style
// Change these for this node
static ADCChannel adcChannelMap[NUM_ADC_CH] = {
	2,		// THERM1
	3,		// THERM2
	5,		// THERM3
	6,		// THERM4
	7,		// THERM5
	8,		// THERM6
	9,		// THERM7
	10		// THERM8
};


#endif /* ADC_CONFIG_H_ */