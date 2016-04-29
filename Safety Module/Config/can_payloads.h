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
#include "cmr_64c1_lib.h"

// Receive mailbox metadata struct
typedef struct ReceiveMeta_t {
	uint16_t missCount;
	uint8_t timeoutFlag;
	uint8_t staleFlag;
	uint8_t differentStateCount;
	uint8_t wrongStateFlag;
} ReceiveMeta_t;

// Heartbeat payload structs
extern SMHeartbeat_t SMHeartbeat;			// Safety Module (self)
extern CCHeartbeat_t CCHeartbeat;			// Central Controller
extern FSMHeartbeat_t FSMHeartbeat;			// Front Sensor Module
extern RSMHeartbeat_t RSMHeartbeat;			// Rear Sensor Module
extern DIMHeartbeat_t DIMHeartbeat;			// Driver Interface Module
extern AFCHeartbeat_t AFCHeartbeat;			// Accumulator Fan Controller
extern TMHeartbeat_t TMHeartbeat;			// Telemetry Module

extern ReceiveMeta_t CCHeartbeatReceiveMeta;
extern ReceiveMeta_t FSMHeartbeatReceiveMeta;
extern ReceiveMeta_t RSMHeartbeatReceiveMeta;
extern ReceiveMeta_t DIMHeartbeatReceiveMeta;
extern ReceiveMeta_t AFCHeartbeatReceiveMeta;
extern ReceiveMeta_t TMHeartbeatReceiveMeta;

// Initialize payloads
void initPayloads(void);

#endif /* CAN_PAYLOADS_H_ */