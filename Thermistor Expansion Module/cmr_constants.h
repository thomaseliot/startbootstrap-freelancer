/*
 * cmr_constants.h
 *
 * Created: 12/10/2015 1:14:50 AM
 *  Author: skir0
 */ 


#ifndef CMR_CONSTANTS_H_
#define CMR_CONSTANTS_H_

// Node state, for main state machine run on all nodes.
typedef enum {GLV_ON, HV_EN, RTD, ERROR} NodeState;
	
// System-wide definitions
#define BRAKE_THRESH	20

#endif /* CMR_CONSTANTS_H_ */