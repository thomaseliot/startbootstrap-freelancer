/*
 * cmr_constants.h
 * 
 * Global constant values
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 


#ifndef CMR_CONSTANTS_H_
#define CMR_CONSTANTS_H_

// Node state, for main state machine run on all nodes.
typedef enum {GLV_ON, HV_EN, RTD, ERROR, CLEAR_ERROR, UNKNOWN} NodeState;

// Status for pumps
typedef enum {PUMP_OFF, PUMP_ON} PumpState;
// Status for fans
typedef enum {FAN_OFF, FAN_RAMPING, FAN_LOW, FAN_HIGH, FAN_ERROR} FanState;


// System-wide definitions
// Brake pressure threshold
#define BRAKE_THRESH		20
// Heartbeat timeout	
#define HEARTBEAT_TIMEOUT	50		// Periods of 10ms
// State timeout
#define STATE_TIMEOUT		500		// Periods of 10ms

#endif /* CMR_CONSTANTS_H_ */
