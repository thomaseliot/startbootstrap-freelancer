/*
 * can_callbacks.c
 * 
 * Callback functions for CAN receive
 *
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include <string.h>

#include "can_callbacks.h"
#include "can_payloads.h"

// This mailbox receives all heartbeats
void heartbeatCallback(CAN_packet packet) {
	// Switch on the packet ID
	switch(packet.id) {
		// Safety Module
		case SM_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(SMHeartbeat_t)) {
				return;
			}
			memcpy(&SMHeartbeat, packet.data, sizeof(SMHeartbeat_t));
			// Set stale flag off
			SMHeartbeatReceiveMeta.staleFlag = 0;
			break;
			
		// Default, don't do anything
		default: break;
	}
}