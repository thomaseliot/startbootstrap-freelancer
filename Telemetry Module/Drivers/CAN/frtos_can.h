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

#ifndef FRTOS_CAN_H
#define FRTOS_CAN_H

#include "can.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define CAN_DUMP_MOB	5

// Mailbox status for receive tasks
typedef struct MOB_STATUS_t {
	uint8_t mob_num;	// Number of this mailbox
	void (*cbk)(CAN_packet p);			// Callback function, this must take a CAN_packet
	uint8_t cnt;		// Generic counter variable
} MOB_STATUS;

// Mailbox callbacks, initialized to NULL. These are assigned in main right now.
// This is an array of function pointers
static void (*MOB_CALLBACKS[NO_MOBS])(CAN_packet p) = {
	NULL, NULL, NULL, NULL, NULL, NULL
};

// Global queue for all CAN packets (CAN dump)
extern xQueueHandle CANdumpQueue;

/* CAN Queue Create
 * Call this function to create a FreeRTOS queue for CAN packet reception
 * All packets matching the id / idmask pattern will be enqueued here.
 * To get the packets simply use xQueueReceive(...)
 * See: xQueueReceive(...)
 * @param id CAN identifier
 * @param id_mask CAN identifier mask
 * @param uxQueueLength number of packets to buffer in queue
 * @return handle of FreeRTOS queue of CAN packets
 */
xQueueHandle xCANQueueCreate(unsigned id, unsigned id_mask, portBASE_TYPE uxQueueLength, unsigned char mob);

/* Get free CAN mailbox
 * Finds first unused CAN mailbox
 * @return mailbox number, or -1 if none found
 */
int get_free_mob();

/* CAN Send
 * Send a single CAN packet
 * @param packet pointer to CAN packet to send
 * @param mob CAN channel to use for this ID (use mob=14 13 12 ...)
 * @param xTicksToWait maximum time before give up (timeout)
 * @return TRUE on successful transmission, FALSE on timeout.
 */			
portBASE_TYPE can_send(CAN_packet *p, unsigned mob, portTickType xTicksToWait);


/* CAN dump init
 * Enable extra queue for reception of all packets 
 * All received packets can be examined using the FreeRTOS queue "CANdumpQueue"
 * @param items buffer size (number of can packets) 
 */			
void can_dump_init(unsigned items);

#endif // FRTOS_CAN_H
