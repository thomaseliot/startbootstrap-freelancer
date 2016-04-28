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
		// Central Controller
		case CC_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(CCHeartbeat_t)) {
				// Fuck
				return;
			}
			// Copy packet contents to global struct
			memcpy(&CCHeartbeat, packet.data, sizeof(CCHeartbeat_t));
			break;
			
		// Front Sensor Module
		case FSM_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(FSMHeartbeat_t)) {
				// Fuck
				return;
			}
			memcpy(&FSMHeartbeat, packet.data, sizeof(FSMHeartbeat_t));
			break;
		
		// Rear Sensor Module
		case RSM_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(RSMHeartbeat_t)) {
				// Fuck
				return;
			}
			memcpy(&RSMHeartbeat, packet.data, sizeof(RSMHeartbeat_t));
			break;
			
		// Driver Interface Module
		case DIM_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(DIMHeartbeat_t)) {
				// Fuck
				return;
			}
			memcpy(&DIMHeartbeat, packet.data, sizeof(DIMHeartbeat_t));
			break;
			
		// Accumulator Fan Controller
		case AFC_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(AFCHeartbeat_t)) {
				// Fuck
				return;
			}
			memcpy(&AFCHeartbeat, packet.data, sizeof(AFCHeartbeat_t));
			break;
			
		// Telemetry Module
		case TM_HEARTBEAT_ID:
			// Make sure data length matches struct size
			if(packet.length != sizeof(TMHeartbeat_t)) {
				// Fuck
				return;
			}
			memcpy(&TMHeartbeat, packet.data, sizeof(TMHeartbeat_t));
			break;
			
		// Default, don't do anything
		default: break;
	}
}