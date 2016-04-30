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
CCHeartbeat_t CCHeartbeat;		// Front Sensor Module

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
	
	// Central Controller Heartbeat
	CCHeartbeat.state = UNKNOWN;
	CCHeartbeat.vbatt = 0;	
}