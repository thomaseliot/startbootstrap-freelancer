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
#define NUM_ADC_CH			3

// Channel enumeration
// Change these for this node
typedef enum ADCChannels {
	BPRES, HE_CUR, VBATT
} ADCChannel;

// Lowest and highest channels, used for loops
// Change these for this node
#define LOWEST_ADC_CH		BPRES
#define HIGHEST_ADC_CH		VBATT

// Channel mapping, symbol to physical ADC channel
// in order of the above enumeration
// TODO: Putting static variables in header files is apparently bad style
// Change these for this node
static ADCChannel adcChannelMap[NUM_ADC_CH] = {
	8,		// BPRES
	9,		// HE_CUR
	10,		// VBATT
};


#endif /* ADC_CONFIG_H_ */