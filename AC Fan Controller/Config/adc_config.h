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
#define NUM_ADC_CH			4

// Channel enumeration
// Change these for this node
typedef enum ADCChannels {
	FAN1_IS, FAN2_IS, FAN3_IS, VBATT
} ADCChannel;

// Lowest and highest channels, used for loops
// Change these for this node
#define LOWEST_ADC_CH		FAN1_IS
#define HIGHEST_ADC_CH		VBATT

// Channel mapping, symbol to physical ADC channel
// in order of the above enumeration
// TODO: Putting static variables in header files is apparently bad style
// Change these for this node
static ADCChannel adcChannelMap[NUM_ADC_CH] = {
	7,  // FAN1_IS
	8,  // FAN2_IS
	9,  // FAN3_IS
	10, // VBATT
};


#endif /* ADC_CONFIG_H_ */