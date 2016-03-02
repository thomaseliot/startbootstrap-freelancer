/*
 * adc.h
 *
 * Analog to digital converter library
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#ifndef ADC_H_
#define ADC_H_

#include "adc_config.h"

// ADC channel metadata
typedef struct ADCMeta_t {
	uint8_t channelNumber;
	uint8_t currentValue;
} ADCMeta;

// Channel data storage
static ADCMeta adcChannels[NUM_ADC_CH];

// Functions
void initADC(void);
uint8_t readADC(uint8_t ch);
void updateADC(ADCChannel ch);
uint8_t adcVal(ADCChannel ch);

#endif /* ADC_H_ */