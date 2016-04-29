/*
 * can_payloads.c
 * 
 * CAN payload objects
 *
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include "can_payloads.h"

// Heartbeat payload structs
SMHeartbeat_t SMHeartbeat;			// Safety Module (self)
CCHeartbeat_t CCHeartbeat;			// Central Controller
FSMHeartbeat_t FSMHeartbeat;		// Front Sensor Module
RSMHeartbeat_t RSMHeartbeat;		// Rear Sensor Module
DIMHeartbeat_t DIMHeartbeat;		// Driver Interface Module
AFCHeartbeat_t AFCHeartbeat;		// Accumulator Fan Controller
TMHeartbeat_t TMHeartbeat;			// Telemetry Module