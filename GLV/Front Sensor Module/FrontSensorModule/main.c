/*
 * FrontSensorModule.c
 *
 * Created: 11/23/2015 5:51:36 PM
 * Author : skir0
 */ 

#include <avr/io.h>
#include "node_tasks.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "can_ids.h"
#include "frtos_can.h"

#define mainMCU_STATUS_TASK_PRIORITY	1
#define mainHEARTBEAT_TASK_PRIORITY		2

//st_cmd_t can_receive_msg;
//uint64_t can_receive_buffer;

int main(void)
{	
	// Initialize CAN with fixed baud rate, as defined in can_config.h
	can_init();
	
	// Setup receive mailbox
	
	
	// Populate message object
	//can_receive_msg.pt_data = &can_receive_buffer	;			// Point to receive buffer
	//can_receive_msg.ctrl.ide = 0;								// CAN 2.0A
	//can_receive_msg.dlc	= 1;								// Data buffer length
	//can_receive_msg.id.std = FSM_HEARTBEAT_ID + 1;				// Set the receive ID to be front sensor module
	//can_receive_msg.cmd = CMD_RX_DATA_MASKED;					// Receive data
	
	/* Wait for an open mailbox and bind msg_id to the mailbox when available*/
	//while(can_cmd(&can_receive_msg) != CAN_CMD_ACCEPTED);
	
	// Create the MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, mainMCU_STATUS_TASK_PRIORITY, NULL);
		
	// Create the heartbeat task, to transmit over CAN
	// Rate: 10Hz
	xTaskCreate(vHeartbeatTask, "HEART", configMINIMAL_STACK_SIZE,
		NULL, mainHEARTBEAT_TASK_PRIORITY, NULL);
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler handles the rest
	return 0;
}

