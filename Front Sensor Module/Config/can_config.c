/* 
 * can_config.c
 * 
 * A configuration file for CAN constants
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include "can_config.h"

// Mailbox directions
uint8_t MOB_DIRS[NO_MOBS] = {
	RX, TX, TX, TX, TX, TX			// 4 RX, 2 TX. Configure based on how much data in/out
};

// Mailbox IDs
uint16_t MOB_IDS[NO_MOBS] = {
	0x200,							// Test message
	NO_ID, NO_ID, NO_ID,
	NO_ID, NO_ID					// TX Mailboxes
};

// Mailbox Masks
// Some useful ones:
//	0x3FF							// Match all
//  0x3F0							// Match upper 7 bits [11:4]
//	0x30F							// Match all but middle four bits [11:8, 3:0]
//	0x0FF							// Match lower 8 bits [7:0]
uint16_t MOB_MASKS[NO_MOBS] = {
	0x3FF,							// Match upper two bits (all 0x200 range)
	NO_MASK, NO_MASK, NO_MASK,
	NO_MASK, NO_MASK				// TX Mailboxes
};

// Mailbox receive task priorities
uint8_t MOB_PRIORITIES[NO_MOBS] = {
	4,								// Change later
	NO_PRIORITY, NO_PRIORITY, NO_PRIORITY,
	NO_PRIORITY, NO_PRIORITY		// TX Mailboxes
};

// Mailbox callback binding
void (*MOB_CALLBACKS[NO_MOBS])(CAN_packet p) = {
	heartbeatCallback,
	NO_CALLBACK, NO_CALLBACK, NO_CALLBACK,
	NO_CALLBACK, NO_CALLBACK		// TX Mailboxes
};