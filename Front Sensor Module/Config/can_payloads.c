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
FSMHeartbeat_t FSMHeartbeat;		// Front Sensor Module

// Metadata for receive messages
ReceiveMeta_t SMHeartbeatReceiveMeta;


/* Initialize payloads
 */
void initPayloads(void) {
	// Safety Module Heartbeat
	SMHeartbeat.state = GLV_ON;
	SMHeartbeat.targetState = GLV_ON;
	SMHeartbeat.vbatt = 0;
	SMHeartbeatReceiveMeta.missCount = 0;
	SMHeartbeatReceiveMeta.timeoutFlag = 0;
	SMHeartbeatReceiveMeta.staleFlag = 1;
	SMHeartbeatReceiveMeta.differentStateCount = 0;
	SMHeartbeatReceiveMeta.wrongStateFlag = 0;
	
	// Accumulator Fan Controller Heartbeat
	FSMHeartbeat.state = UNKNOWN;
	FSMHeartbeat.vbatt = 0;
	FSMHeartbeat.requestedTorque = 0;
	
}