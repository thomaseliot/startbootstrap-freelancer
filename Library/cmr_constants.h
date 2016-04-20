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
typedef enum {STATE_GLV_ON, STATE_HV_EN, STATE_RTD, STATE_ERROR, STATE_UNKNOWN} NodeState;

// System-wide definitions
#define BRAKE_THRESH	20

#endif /* CMR_CONSTANTS_H_ */
