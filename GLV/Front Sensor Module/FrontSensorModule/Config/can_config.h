/*
 * can_config.h
 * 
 * A configuration file for CAN constants such as baudrate and oscillator frequency
 * 
 * Created: 2/22/2016
 * Author: Shepard Emerson (semerson)
 * 
 * Known issues:
 *	- None
 */

#ifndef CAN_CONFIG_H_
#define CAN_CONFIG_H_

// Includes
// #include <avr/io.h>

/*
#define FOSC			16000
#define F_CPU          (FOSC*1000) // Need for AVR GCC
*/

// Length of each receive queue
#define CAN_QUEUE_LEN	5

// Mailbox Configuration
// Direction definitions (TX or RX)
#define TX				0
#define RX				1

// No ID/Mask definitions
#define NO_ID			0xFFF		// Both of these are out of range
#define NO_MASK			0xFFF		// of 11-bit (0x7FF)
#define NO_PRIORITY		0

// Mailbox directions
volatile static uint8_t MOB_DIRS[NO_MOBS] = {
	RX, RX, RX, RX, TX, TX
};

// Mailbox IDs
volatile static uint16_t MOB_IDS[NO_MOBS] = {
	0x100,							// Test message
	0x101,							// Test message
	0x102,							// Test message
	0x103,							// Test message
	NO_ID, NO_ID					// TX Mailboxes
};

// Mailbox Masks
// Some useful ones:
//	0x3FF							// Match all
//  0x3F0							// Match upper 7 bits [11:4]
//	0x30F							// Match all but middle four bits [11:8, 3:0]
//	0x0FF							// Match lower 8 bits [7:0]
volatile static uint16_t MOB_MASKS[NO_MOBS] = {
	0x3FF,							// Match all
	0x3FF,							// Match all 
	0x3FF,							// Match all
	0x3FF,							// Match all
	NO_MASK, NO_MASK				// TX Mailboxes
};

// Mailbox priorities
volatile static uint8_t MOB_PRIORITIES[NO_MOBS] = {
	3,								// Change later
	3,
	3,
	3,
	NO_PRIORITY, NO_PRIORITY		// TX Mailboxes
};

// Mailbox callbacks, initialized to NULL
volatile static void (*MOB_CALLBACKS[NO_MOBS])(CAN_packet p) = {
	NULL, NULL, NULL, NULL, NULL, NULL
};

#endif /* CAN_CONFIG_H_ */