/* node_config.h
 * 
 * Physical configuration for this node. Place all port/channel definitions
 * here, as well as oscillator frequency.
 * 
 */ 


#ifndef NODE_CONFIG_H_
#define NODE_CONFIG_H_

#include "cmr_64c1_lib.h"

// Oscillator Frequency
#define F_OSC				16000000

// MCU Status
#define MCU_STATUS_PORT		IO_PORT_C
#define MCU_STATUS_CH		0

// External Watchdog Kick
#define EXT_WD_KICK_PORT	IO_PORT_D
#define EXT_WD_KICK_CH		1

// Thermistor Fault Controls
#define OPEN_FAULT_PORT		IO_PORT_C		// Open circuit on thermistor output
#define OPEN_FAULT_CH		7
#define SHORT_FAULT_PORT	IO_PORT_D		// Short circuit on thermistor output
#define SHORT_FAULT_CH		0

#endif /* NODE_CONFIG_H_ */