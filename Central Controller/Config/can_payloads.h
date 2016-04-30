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
extern CCHeartbeat_t CCHeartbeat;			// Accumulator Fan Controller


extern ReceiveMeta_t SMHeartbeatReceiveMeta;

// Initialize payloads
void initPayloads(void);

#endif /* CAN_PAYLOADS_H_ */