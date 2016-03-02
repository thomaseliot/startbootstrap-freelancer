/*
 * FrontSensorModule.c
 *
 * Created: 11/23/2015 5:51:36 PM
 */ 

#include <avr/io.h>
#include <stdio.h>
#include "node_tasks.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "can_ids.h"
#include "frtos_can.h"
#include "can_config.h"
#include "can_callbacks.h"

#define mainMCU_STATUS_TASK_PRIORITY	1
#define mainHEARTBEAT_TASK_PRIORITY		2

// Global status variable
MOB_STATUS statuses[NO_MOBS];

//st_cmd_t can_receive_msg;
//uint64_t can_receive_buffer;

int main(void)
{	
	// Function variables
	uint16_t i;
	
	// Initialize CAN
	can_init();
	
	// Create the MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, mainMCU_STATUS_TASK_PRIORITY, NULL);
	
	// Assign mailbox callbacks
	statuses[0].cbk = mb1_callback;
	statuses[1].cbk = mb2_callback;
	statuses[2].cbk = mb3_callback;
	statuses[3].cbk = mb4_callback;
	
	
	// Create tasks for receive mailboxes
	for(i = 0; i < NO_MOBS; i++) {
		
		// Initialize mailbox status
		statuses[i].mob_num = i;
		statuses[i].cnt = 0;
		
		if(MOB_DIRS[i] == RX) {
			// Name of task
			char name[4];
			sprintf(name, "RCV%d", i);
			// Create task for this mailbox
			xTaskCreate(vCANReceiveTask, name, configMINIMAL_STACK_SIZE,
				(void *)i, MOB_PRIORITIES[i], NULL);
		}
	}
	
	// Create the heartbeat task, to transmit over CAN
	// Rate: 10Hz
	xTaskCreate(vHeartbeatTask, "HEART", configMINIMAL_STACK_SIZE,
		statuses, mainHEARTBEAT_TASK_PRIORITY, NULL);
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler handles the rest
	return 0;
}

