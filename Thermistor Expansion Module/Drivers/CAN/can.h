/*
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
#include <stdint.h>
#include <stdbool.h>

#ifndef CAN_H_
#define CAN_H_

#define NO_MOBS 6 		// number of message object buffers (MOBs) (ATmega64c1)
#define NOMOB 	0xff 	// return value for getmob()
#define RTR		0xff 	// special value for length -> RTR packet

// CAN packet structure
// Used to describe a CAN20a (standard 11 bit) packet.
// A packet length of 0 indicates an RTR packet
typedef struct {
	unsigned id;			// CAN identifier
	unsigned char length;	// CAN data length
	unsigned char data[8];	// CAN data array [8]
} CAN_packet;

// CAN callback function for RX
// Signature of functions that will be called on a CAN receiver interrupt if 
// the corresponding id/mask pattern was enabled.
typedef void (* CAN_cbf)(CAN_packet *p, unsigned char mob);

/* CAN Receive Routine
 * 
 * 
 * @param mob message object buffer to use (1..14)
 * @param id CAN identifier / id range
 * @param idmask mask telling which bits in id to check
 * @param callback pointer to callback function (isr callback)
 * @return FALSE if MOB not ready, TRUE otherwise
*/
bool prepare_rx(char mob, unsigned id, unsigned idmask, CAN_cbf callback);

/* CAN Transmit Routine
 * 	
 * @param mob MOB channel to use. 
 * 		The receive function uses low MOB numbers 0,1, ...
 * 		Therefore use high numbers 15, 14 ... for transmission.
 * 		Ideally, use different MOBs for different IDs to maintain priority.
 * @param packet pointer to CAN packet
 * @return FALSE if TX on this MOB not ready, TRUE otherwise
*/
bool can_tx(char mob, CAN_packet *packet);

/* CAN Interface Initialization
 * 
 * Call this function before any CAN related function is used.
 */
void can_init(void);

#endif