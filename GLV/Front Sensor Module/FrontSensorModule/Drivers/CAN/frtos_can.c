/*!
	\file frtos_can.c 
	\brief interface between the generic CAN driver can.c and FreeRTOS

	message - driven reception of CAN packets.\n
	polled transmission of CAN packets with very little CPU overhead.

	\author Dr. Klaus Schaefer \n
	Hochschule Darmstadt * University of Applied Sciences \n
	schaefer@eit.h-da.de \n
	http://kschaefer.eit.h-da.de

	You can redistribute it and/or modify it 
	under the terms of the GNU General Public License
	It is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
*/
#include "frtos_can.h"
#include <assert.h>

static void CAN_callback( CAN_packet *p, unsigned char mob);
xQueueHandle CANdumpQueue; //!< Global queue for all CAN packets (CAN dump)
xQueueHandle channels[NO_MOBS]; //!< mob -> FreeRTOS Queue list

void can_dump_init( unsigned items)
	{
	CANdumpQueue = xQueueCreate( items, sizeof( CAN_packet));
	assert( CANdumpQueue !=0);
// catch all "floating" IDs using MOB 14
	channels[ 14]=CANdumpQueue;
	assert( 0 == prepare_rx( 14, 0x7ff, 0x000, CAN_callback));
	}

/*! callback routine for interrupts of all received can packets

	this routine scans the list of all MOBs 
	to find the responsible FreeRTOS queue to enqueue this CAN packet to.
*/
static void CAN_callback( CAN_packet *p, unsigned char mob)
	{
	portBASE_TYPE xTaskWoken = pdFALSE;
	assert( channels[mob] !=0);
	xQueueSendFromISR( channels[mob], p, &xTaskWoken);
	if( CANdumpQueue && (mob!=14)) // do not double-dump 14, packet already sent!
		xQueueSendFromISR( CANdumpQueue, p, &xTaskWoken);
	if( xTaskWoken) 
		taskYIELD();
	}

xQueueHandle xCANQueueCreate( unsigned id, unsigned idmask, portBASE_TYPE uxQueueLength)
	{
	xQueueHandle theQ;
	unsigned mob;
//	taskENTER_CRITICAL();
	theQ= xQueueCreate( uxQueueLength, sizeof( CAN_packet));
	if( theQ==0)
		return theQ;
	// find free mob
	for( mob=0; mob < NO_MOBS; ++mob)
		if( channels[mob]==0)
			break;
	if( mob==NO_MOBS)
		return 0; // out of MOBs
	channels[ mob]=theQ;
	prepare_rx( mob, id, idmask, CAN_callback);
//	taskEXIT_CRITICAL();
	return theQ;
	}

portBASE_TYPE can_send( CAN_packet *p, unsigned mob, portTickType xTicksToWait)
	{
	char retv=false;

	assert( channels[mob]==(CAN_cbf)0); // must be vacated
	channels[mob]=(CAN_cbf)0xffff; // this mob is in use now!

	while( xTicksToWait--)
		{
		retv=can_tx( mob, p);
		if( retv==true) // transmission succsessful
			{
			if( CANdumpQueue)
				xQueueSend( CANdumpQueue, p, xTicksToWait);
			break;
			}
		// this method is not really elegant
		// but it is save and simple
		// since in usual systems 
		// this "busy" waiting will not be used at all
		vTaskDelay(1);
		}
	
	channels[mob]=(CAN_cbf)0; // mob vacated

	return  retv;
	}
