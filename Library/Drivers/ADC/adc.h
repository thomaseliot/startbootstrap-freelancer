/*
 * adc.h
 *
 * ADC interrupt and configuration functions
 * 
 */ 

#ifndef ADC_H_
#define ADC_H_

#include "adc_config.h"

// ADC channel metadata
typedef struct ADCMeta_t {
	uint8_t channelNumber;
	uint16_t currentValue;
} ADCMeta;

// Channel data storage
static ADCMeta adcChannels[NUM_ADC_CH];

// Functions
void initADC(void);
uint16_t readADC(uint8_t ch);
void updateADC(ADCChannel ch);
uint16_t adcVal(ADCChannel ch);

#endif /* ADC_H_ */