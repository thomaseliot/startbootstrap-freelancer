/*
 * spi_config.h
 * 
 * SPI configuration for this node
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#ifndef SPI_CONFIG_H_
#define SPI_CONFIG_H_

#include "cmr_64c1_lib.h"

// Number of slaves
#define NUM_SLAVES			2

// Slave enumeration
typedef enum SPISlaveEnum {
	POT1_SS, POT2_SS
};

// Struct for storing port/pin of slave
typedef struct SPISlave_t {
	uint8_t port;
	uint8_t pin;
} SPISlave;

// Slave configuration array
static SPISlave spiSlaves[NUM_SLAVES] = {
	{IO_PORTC, 1},	// POT1, Port C1
	{IO_PORTC, 2}	// POT2, Port C2
};


#endif /* SPI_CONFIG_H_ */