/*
 * can_structs.h
 * Structs used by the Front Sensor Module for CAN messages.
 */ 

#ifndef CAN_STRUCTS_H_
#define CAN_STRUCTS_H_

#include "cmr_constants.h"
#include "stdint.h"

typedef struct HeartbeatFSM_t {
	NodeState currentState;
	uint8_t someData;
} HeartbeatFSM;



#endif /* CAN_STRUCTS_H_ */