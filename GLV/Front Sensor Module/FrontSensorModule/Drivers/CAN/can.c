/*
	Generic CAN bus driver for AT90CAN128, adapted for ATmega64c1.

	Provides interrupt-driven reception and polled transmission of CAN packets

	Author: Dr. Klaus Schaefer
	Hochschule Darmstadt * University of Applied Sciences
	schaefer@eit.h-da.de
	http://kschaefer.eit.h-da.de
	
	Modified By: Shepard Emerson, Carnegie Mellon Racing
	Ported to ATmega64c1 and added comments

	You can redistribute it and/or modify it under the terms of the GNU General 
	Public License. It is distributed in the hope that it will be useful, but 
	WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
	or FITNESS FOR A PARTICULAR PURPOSE.
	See http://www.gnu.org/copyleft/gpl.html for more details.
*/

#include "avr/interrupt.h"
#include <assert.h>
#include "can.h"

// CAN callback list for RX channels, indexed by mailbox number
CAN_cbf canlist[NO_MOBS];

// CAN interrupt service routine
// Used for reception of CAN packets
ISR(CAN_INT_vect) {
	// Function variables
	unsigned i;
	static CAN_packet packet;
	char save_canpage = CANPAGE;
	
	// Get highest priority mailbox
	unsigned mob = CANHPMOB;
	// Set the CAN page to this mailbox
    CANPAGE = mob & 0xf0;	// note mob is currently upper four bits
	// Shift mailbox number to the lower four bits
	mob >>= 4;;
	
	// Allow only RX ready and DLC warning
	assert((CANSTMOB & ~0xa0) == 0);
	
	register char length;
	// Get ID of incoming message
	packet.id = (CANIDT2 >> 5) | (CANIDT1 << 3);
	// Get length of incoming message and set in packet
	length = CANCDMOB & 0x0f;
	packet.length = length;
	
	// Get incoming data
	for (i = 0; i < length; ++i) {
		// CANMSG index is autoincremented on read
		packet.data[i] = CANMSG;
	}
	
	// Reset interrupt flags
	CANSTMOB = 0;
	// Re-enable RX on this channel
	CANCDMOB=0x80;
	
	// Call the mailbox callback
	canlist[mob](&packet, mob);
	
	// Restore CANPAGE
	CANPAGE = save_canpage;
}

/* Transmit over CAN
 * Transmits the given packet using specified mailbox
 * @param mob: the mailbox to use
 * @param packet: pointer to the packet to send
 */
bool can_tx( char mob, 	CAN_packet *packet ) {
	// Counter variable
	unsigned cnt;
	
	// Ensure valid packet
	assert(packet->id <= 0x7ff);
	assert(packet->length <=8);
	assert(mob <=14);
	
	// Set the CAN page to this mailbox
    CANPAGE = mob << 4;
	
	// If MOB in use and TX not ready
	if((CANCDMOB & 0b11000000) && ((CANSTMOB & 0x40) == 0)) {
		// Failure
		return false;
	}
	
	// Cancel pending operation
	CANSTMOB = 0x00; 
	CANCDMOB = 0x00;
	
	// Support for remote transmission request (RTR)
	if(packet->length != RTR) {
		// Set the ID to send
		CANIDT1 = packet->id >> 3;
		CANIDT2 = packet->id << 5;
        CANIDT3 = 0;
        CANIDT4 = 0;
		
		// Set the data
		for (cnt=0; cnt < packet->length; ++cnt) {
			// CANMSG auto increments
			CANMSG = packet->data[cnt];
		}
		
		// Set message length
    	CANCDMOB |= packet->length;
		
	} else {
		// Set the ID to send
		CANIDT1=packet->id >> 3;
		CANIDT2=packet->id << 5;
        CANIDT3 = 0;
        CANIDT4 = 1<<RTRTAG;	// RTR
		
		// Set the packet length
		CANCDMOB |= packet->length;
	}
	
	// Enable TX
    CANCDMOB|=0x40;
	
	// Success
	return true;
}

/* Receive over CAN
 * Sets up a receive mailbox
 * @param mob: the mailbox to use
 * @param id: the ID to send the message with
 * @param idmask: the CAN mask to use
 * @param callback the callback function to call when a receive happens
 */
bool prepare_rx( char mob, unsigned id, unsigned idmask, CAN_cbf callback) {
	// Protect against illegal mailbox number
	if(mob >= 15) {
		// True indicates error
		return true;
	}
	
	// Add the callback to the CAN list
	canlist[(unsigned)mob] = callback;
	
	// Set the CAN page
    CANPAGE = mob << 4;
	
	// Release mailbox if requested
	if(callback == 0) { 
		// Cancel pending operation
		CANSTMOB = 0x00;
		CANCDMOB = 0x00;
		// Disable interrupt for this mailbox
		unsigned mask = 1 << mob;
		CANIE2 &= ~mask;
		// Success
		return false;
	} 
	
	// See if the mailbox is in use
	if( CANCDMOB & 0b11000000) {
		// Error
		return true;
	}
	
	// Mailbox isn't in use
	// Cancel pending operation
	CANSTMOB = 0x00;
	CANCDMOB = 0x00;
	// Enable direct mailbox indexing
	CANHPMOB = 0x00;
	
	// Set ID to receive
	CANIDT1 = id >> 3;
	CANIDT2 = id << 5;
	// Set receive mask
	CANIDM1=idmask >> 3;
	CANIDM2=idmask << 5;
	CANIDM3=0;
	CANIDM4=0;
	
	// Enable RX
	CANCDMOB = 0x80;
	// Enable interrupt for this mailbox
	unsigned mask = 1 << mob;
	CANIE2 |= mask;
	CANIE1 |= (mask >> 8);
	
	// Return false indicating success
	return false;
}

/* Initialize CAN
 * Initialize to baud rate and clear registers
 */
void can_init( void) {
	// Function variable
	unsigned mob;

	// Set baudrate to 500kbit/s
	// ASSUMES 16MHz
	CANBT1=0x06;	// 16 MHz, 500kbit/s
	CANBT2=0x04;
	CANBT3=0x13;
	
	// Clear registers
	for (mob = 0; mob < NO_MOBS; mob++) {
		CANPAGE  = (mob << 4);
		CANSTMOB = 0;
		CANCDMOB = 0;
	}
	
	// Enable CAN
	CANGCON |= 0x02;
	// Enable receive interrupt
	CANGIE = (1 << ENIT) | (1 << ENRX);
}

