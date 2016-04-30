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
#define NUM_ADC_CH			6

// Channel enumeration
// Change these for this node
typedef enum ADCChannels {
	SNS_SIG2, SNS_SIG3, SNS_SIG4, SNS_SIG5, FAN_IS, VBATT
} ADCChannel;

// Lowest and highest channels, used for loops
// Change these for this node
#define LOWEST_ADC_CH		SNS_SIG2
#define HIGHEST_ADC_CH		VBATT

// Channel mapping, symbol to physical ADC channel
// in order of the above enumeration
// TODO: Putting static variables in header files is apparently bad style
// Change these for this node
static ADCChannel adcChannelMap[NUM_ADC_CH] = {
	2,	//SNS_SIG2
	3,	//SNS_SIG2
	4,	//SNS_SIG2
	5,	//SNS_SIG2
	9,  // FAN_IS
	10 // VBATT
};


#endif /* ADC_CONFIG_H_ */