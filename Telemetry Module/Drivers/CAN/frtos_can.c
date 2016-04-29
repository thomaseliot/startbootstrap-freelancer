/*!
	frtos_can.c 
	Interface between the generic CAN driver can.c and FreeRTOS

	Message - driven reception of CAN packets.
	Polled transmission of CAN packets with very little CPU overhead.

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
#include "frtos_can.h"
#include <assert.h>

static void CAN_callback(CAN_packet *p, unsigned char mob);
xQueueHandle CANdumpQueue;			// Global queue for all CAN packets (CAN dump)
xQueueHandle channels[NO_MOBS];		// mob -> FreeRTOS Queue list


/* CAN dump init
 * Enable extra queue for reception of all packets 
 * All received packets can be examined using the FreeRTOS queue "CANdumpQueue"
 * @param items buffer size (number of can packets) 
 */	
void can_dump_init(unsigned items) {
	// Create queue to receive all packets
	CANdumpQueue = xQueueCreate(items, sizeof(CAN_packet));
	assert(CANdumpQueue !=0);
	
	// Catch all "floating" IDs using CAN_DUMP_MOB
	channels[CAN_DUMP_MOB] = CANdumpQueue;
	
	// Register receive mailbox
	assert(0 == prepare_rx(CAN_DUMP_MOB, 0x7ff, 0x000, CAN_callback));
}


/* CAN callback
 * Callback routine for interrupts of all received CAN packets
 * This routine scans the list of all MOBs to find the responsible FreeRTOS 
 * queue to enqueue this CAN packet to.
 */
static void CAN_callback(CAN_packet *p, unsigned char mob) {
	// Task previously woken varaible
	portBASE_TYPE xTaskWoken = pdFALSE;
	
	// Make sure queue for this mailbox is set up
	assert(channels[mob] != 0);
	
	// Send the packet to the queue
	xQueueSendFromISR(channels[mob], p, &xTaskWoken);
	
	// Send to dump queue if set up, making sure not to double-send
	if(CANdumpQueue && (mob != CAN_DUMP_MOB)) {
		xQueueSendFromISR(CANdumpQueue, p, &xTaskWoken);
	}
	
	// If posting to this queue woke a task, then immediately perform a context switch
	if(xTaskWoken)
		taskYIELD();
}


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
xQueueHandle xCANQueueCreate(unsigned id, unsigned idmask, portBASE_TYPE uxQueueLength, unsigned char mob) {
	// Function variables
	xQueueHandle theQ;
	
	// Uncomment to make this a critical section
	// taskENTER_CRITICAL();
	
	// Create the queue
	theQ = xQueueCreate(uxQueueLength, sizeof(CAN_packet));
	
	// Check to see if queue was successfully made
	if(theQ == 0)
		return theQ;	// If unsuccessful return zero now
		
	// Make sure this is a free mailbox
	if(channels[mob] != 0) {
		return 0;
	}
	
	// Associate queue with free mailbox found and setup receive
	channels[mob] = theQ;
	prepare_rx(mob, id, idmask, CAN_callback);
	
	// Uncomment to make this a critical section
	// taskEXIT_CRITICAL();
	
	// Return the successfully created queue
	return theQ;
}

/* Get free CAN mailbox
 * Finds first unused CAN mailbox
 * @return mailbox number, or -1 if none found
 */
int get_free_mob() {
	// Function variable
	unsigned mob;
	
	// Find free mailbox
	for(mob = 0; mob < NO_MOBS; ++mob) {
		// Break if this mailbox is free and a transmit mailbox
		if(channels[mob] == 0 && MOB_DIRS[mob] == TX) {
			break;
		}
	}
	
	// Return -1 if out of mailboxes
	if(mob == NO_MOBS)
		return -1;
		
	// Else return the mailbox
	return mob;
}


/* CAN Send
 * Send a single CAN packet
 * @param packet pointer to CAN packet to send
 * @param mob CAN channel to use for this ID (use mob=14 13 12 ...)
 * @param xTicksToWait maximum time before give up (timeout)
 * @return TRUE on successful transmission, FALSE on timeout.
 */	
portBASE_TYPE can_send(CAN_packet *p, unsigned mob, portTickType xTicksToWait) {
	// Function variable, default return value of failure
	char retv = false;

	// Make sure the specified mailbox is vacated
	assert(channels[mob] == (CAN_cbf)0);
	
	// Set mailbox as in use
	channels[mob]=(CAN_cbf)0xffff;
	
	// Wait for timeout
	while(xTicksToWait--) {
		// Try to transmit packet on specified mob
		retv = can_tx(mob, p);
		
		// See if transmission successful
		if(retv == true) {
			// Send to dump queue if enabled
			if(CANdumpQueue)
				xQueueSend(CANdumpQueue, p, xTicksToWait);
			// Break out of loop
			break;
		}
		// This method is not really elegant, but it is safe and simple, since 
		// in usual systems this "busy" waiting will not be used at all
		vTaskDelay(1);
	}
	
	// Vacate mailbox
	channels[mob]=(CAN_cbf)0;

	return retv;
}
