/*
 * cmr_constants.h
 * 
 * Author: semerson
 */ 


#ifndef CMR_CONSTANTS_H_
#define CMR_CONSTANTS_H_

// Node state, for main state machine run on all nodes.
typedef enum {GLV_ON, HV_EN, RTD, ERROR} NodeState;

// System-wide definitions
#define BRAKE_THRESH	20

#endif /* CMR_CONSTANTS_H_ */