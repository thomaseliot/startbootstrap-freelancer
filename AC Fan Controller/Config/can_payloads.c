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

// Metadata for receive messages
ReceiveMeta_t CCHeartbeatReceiveMeta;
ReceiveMeta_t FSMHeartbeatReceiveMeta;
ReceiveMeta_t RSMHeartbeatReceiveMeta;
ReceiveMeta_t DIMHeartbeatReceiveMeta;
ReceiveMeta_t AFCHeartbeatReceiveMeta;
ReceiveMeta_t TMHeartbeatReceiveMeta;
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
	AFCHeartbeat.state = UNKNOWN;
	AFCHeartbeat.fan1Status = FAN_OFF;
	AFCHeartbeat.fan1Current = 0;
	AFCHeartbeat.fan2Status = FAN_OFF;
	AFCHeartbeat.fan2Current = 0;
	AFCHeartbeat.fan3Status = FAN_OFF;
	AFCHeartbeat.fan3Current = 0;
	AFCHeartbeat.vbatt = 0;
	
}