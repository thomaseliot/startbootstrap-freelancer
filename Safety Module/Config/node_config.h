/* node_config.h
 * 
 * Physical configuration for this node. Place all port/channel definitions
 * here, as well as oscillator frequency.
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#ifndef NODE_CONFIG_H_
#define NODE_CONFIG_H_

#include "cmr_64c1_lib.h"

// Oscillator Frequency
#define F_OSC				16000000		// 16MHz

// MCU Status
#define MCU_STATUS_PORT		IO_PORTC
#define MCU_STATUS_CH		0

// BSPD and IMD errors
#define BSPD_ERR_PORT		IO_PORTB
#define BSPD_ERR_CH			1
#define IMD_ERR_PORT		IO_PORTD
#define IMD_ERR_CH			1

// DC/DC faults
#define DCDC1_FAULT_PORT	IO_PORTB
#define DCDC1_FAULT_CH		6
#define DCDC2_FAULT_PORT	IO_PORTB
#define DCDC2_FAULT_CH		5

// Define the rest of your I/O configuration here

#endif /* NODE_CONFIG_H_ */