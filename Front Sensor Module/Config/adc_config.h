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
#define NUM_ADC_CH			9

// Channel enumeration
// Change these for this node
typedef enum ADCChannels {
	DPOS_FL,
	DPOS_FR,
	TPOS_L,
	TPOS_R,
	SW_ANGLE,
	BPRES_F,
	WSPD_FL,
	WSPD_FR,
	VBATT
} ADCChannel;

// Lowest and highest channels, used for loops
// Change these for this node
#define LOWEST_ADC_CH		DPOS_FL
#define HIGHEST_ADC_CH		VBATT

// Channel mapping, symbol to physical ADC channel
// in order of the above enumeration
// TODO: Putting static variables in header files is apparently bad style
// Change these for this node
static ADCChannel adcChannelMap[NUM_ADC_CH] = {
	2,		// DPOS_FL
	3,		// DPOS_FR
	4,		// TPOS_L
	5,		// TPOS_R
	6,		// SW_ANGLE
	7,		// BPRES_F
	8,		// WSPD_FL
	9,		// WSPD_FR
	10		// VBATT
};


#endif /* ADC_CONFIG_H_ */