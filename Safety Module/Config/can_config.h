/* 
 * can_config.h
 * 
 * A configuration file for CAN constants
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#ifndef CAN_CONFIG_H_
#define CAN_CONFIG_H_

#include "can.h"
#include "can_callbacks.h"

// Length of each receive queue
#define CAN_QUEUE_LEN	5			// Keep this small, or you will eat memory

// Direction definitions (TX or RX)
#define TX				0
#define RX				1

// No ID/Mask definitions
#define NO_ID			0xFFF		// Both of these are out of range
#define NO_MASK			0xFFF		// of 11-bit (0x7FF)
#define NO_PRIORITY		0
#define NO_CALLBACK		NULL		

// Mailbox directions
extern uint8_t MOB_DIRS[NO_MOBS];
// Mailbox IDs
extern uint16_t MOB_IDS[NO_MOBS];
// Mailbox Masks
extern uint16_t MOB_MASKS[NO_MOBS];
// Mailbox receive task priorities
extern uint8_t MOB_PRIORITIES[NO_MOBS];
// Mailbox callback binding
extern void (*MOB_CALLBACKS[NO_MOBS])(CAN_packet p);

#endif /* CAN_CONFIG_H_ */