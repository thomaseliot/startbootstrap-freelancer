/*!
	\author Dr. Klaus Schaefer \n
	Hochschule Darmstadt * University of Applied Sciences \n
	schaefer@eit.h-da.de \n
	http://kschaefer.eit.h-da.de


	You can redistribute it and/or modify it 
	under the terms of the GNU General Public License.\n
	It is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; \n
	without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n
	See http://www.gnu.org/copyleft/gpl.html for mor details.
*/

#ifndef FRTOS_CAN_H
#define FRTOS_CAN_H

#include "can.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

extern xQueueHandle CANdumpQueue; //!< Global queue for all CAN packets (CAN dump)

//!	call this function to create a FreeRTOS queue for CAN packet reception

/*!
	all packets mtching the id / idmask pattern will be enqueued here.
	To get the packets simply use xQueueReceive(...)
	\see xQueueReceive(...)
	\param id CAN identifier
	\param id_mask CAN identifier mask
	\param uxQueueLength number of packets to buffer in queue
	\return handle of FreeRTOS queue of CAN packets
*/
xQueueHandle xCANQueueCreate( unsigned id, unsigned id_mask, portBASE_TYPE uxQueueLength);

//! send a single CAN packet

/*! 
	\param packet pointer to CAN packet to send
	\param mob CAN channel to use for this ID (use mob=14 13 12 ...)
	\param xTicksToWait maximum time befor give up (timeout)
	\return TRUE on sucsessful transmission, FALSE on timeout.
*/			
portBASE_TYPE can_send( CAN_packet *p, unsigned mob, portTickType xTicksToWait);

/*! 
@brief enable extra queue for reception of all packets 

All received packets can be examiated 
using the FreeRTOS queue "CANdumpQueue"

	\param items buffer size (number of can packets) 
*/			
void can_dump_init( unsigned items);

#endif // FRTOS_CAN_H
