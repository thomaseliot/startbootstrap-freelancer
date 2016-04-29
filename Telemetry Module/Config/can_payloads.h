/*
 * can_payloads.h
 *
 * CAN payload objects
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 


#ifndef CAN_PAYLOADS_H_
#define CAN_PAYLOADS_H_

#include "can_structs.h"

// Heartbeat payload structs
extern SMHeartbeat_t SMHeartbeat;			// Safety Module (self)
extern CCHeartbeat_t CCHeartbeat;			// Central Controller
extern FSMHeartbeat_t FSMHeartbeat;			// Front Sensor Module
extern RSMHeartbeat_t RSMHeartbeat;			// Rear Sensor Module
extern DIMHeartbeat_t DIMHeartbeat;			// Driver Interface Module
extern AFCHeartbeat_t AFCHeartbeat;			// Accumulator Fan Controller
extern TMHeartbeat_t TMHeartbeat;			// Telemetry Module


#endif /* CAN_PAYLOADS_H_ */