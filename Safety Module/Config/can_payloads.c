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

/* Initialize payloads
 */
void initPayloads(void) {
	// Safety Module Heartbeat
	SMHeartbeat.state = GLV_ON;
	SMHeartbeat.targetState = GLV_ON;
	SMHeartbeat.vbatt = 0;
	
	// Central Controller Heartbeat
	CCHeartbeat.state = UNKNOWN;
	CCHeartbeat.vbatt = 0;
	CCHeartbeatReceiveMeta.missCount = 0;
	CCHeartbeatReceiveMeta.timeoutFlag = 0;
	CCHeartbeatReceiveMeta.staleFlag = 1;
	CCHeartbeatReceiveMeta.differentStateCount = 0;
	CCHeartbeatReceiveMeta.wrongStateFlag = 0;
	
	// Front Sensor Module Heartbeat
	FSMHeartbeat.state = UNKNOWN;
	FSMHeartbeat.vbatt = 0;
	FSMHeartbeat.requestedTorque = 0;
	FSMHeartbeatReceiveMeta.missCount = 0;
	FSMHeartbeatReceiveMeta.timeoutFlag = 0;
	FSMHeartbeatReceiveMeta.staleFlag = 1;
	FSMHeartbeatReceiveMeta.differentStateCount = 0;
	FSMHeartbeatReceiveMeta.wrongStateFlag = 0;
	
	// Rear Sensor Module Heartbeat
	RSMHeartbeat.state = UNKNOWN;
	RSMHeartbeat.radiatorFanStatus = FAN_OFF;
	RSMHeartbeat.radiatorFanCurrent = 0;
	RSMHeartbeat.leftPumpStatus = PUMP_OFF;
	RSMHeartbeat.rightPumpStatus = PUMP_OFF;
	RSMHeartbeat.vbatt = 0;
	RSMHeartbeatReceiveMeta.missCount = 0;
	RSMHeartbeatReceiveMeta.timeoutFlag = 0;
	RSMHeartbeatReceiveMeta.staleFlag = 1;
	RSMHeartbeatReceiveMeta.differentStateCount = 0;
	RSMHeartbeatReceiveMeta.wrongStateFlag = 0;
	
	// Driver Interface Module Heartbeat
	DIMHeartbeat.state = UNKNOWN;
	DIMHeartbeat.requestedState = UNKNOWN;
	DIMHeartbeat.vbatt = 0;
	DIMHeartbeatReceiveMeta.missCount = 0;
	DIMHeartbeatReceiveMeta.timeoutFlag = 0;
	DIMHeartbeatReceiveMeta.staleFlag = 1;
	DIMHeartbeatReceiveMeta.differentStateCount = 0;
	DIMHeartbeatReceiveMeta.wrongStateFlag = 0;
	
	// Accumulator Fan Controller Heartbeat
	AFCHeartbeat.state = UNKNOWN;
	AFCHeartbeat.fan1Status = FAN_OFF;
	AFCHeartbeat.fan1Current = 0;
	AFCHeartbeat.fan2Status = FAN_OFF;
	AFCHeartbeat.fan2Current = 0;
	AFCHeartbeat.fan3Status = FAN_OFF;
	AFCHeartbeat.fan3Current = 0;
	AFCHeartbeat.vbatt = 0;
	AFCHeartbeatReceiveMeta.missCount = 0;
	AFCHeartbeatReceiveMeta.timeoutFlag = 0;
	AFCHeartbeatReceiveMeta.staleFlag = 1;
	AFCHeartbeatReceiveMeta.differentStateCount = 0;
	AFCHeartbeatReceiveMeta.wrongStateFlag = 0;
	
	// Telemetry Module Heartbeat
	TMHeartbeat.state = UNKNOWN;
	TMHeartbeat.vbatt = 0;
	TMHeartbeatReceiveMeta.missCount = 0;
	TMHeartbeatReceiveMeta.timeoutFlag = 0;
	TMHeartbeatReceiveMeta.staleFlag = 1;
	TMHeartbeatReceiveMeta.differentStateCount = 0;
	TMHeartbeatReceiveMeta.wrongStateFlag = 0;
}