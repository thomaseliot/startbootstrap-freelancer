/* node_tasks.c
 * Contains code for all tasks
 * 
 * Copyright (c) 2016, Carnegie Mellon Racing
 */ 


#include <avr/io.h>
#include <string.h>
#include <stdbool.h>
#include "node_tasks.h"
#include "task.h"
#include "queue.h"
#include "can_ids.h"
#include "can_config.h"
#include "cmr_64c1_lib.h"
#include "node_config.h"
#include "frtos_can.h"
#include "assert.h"

/* MCU Status task
 * Toggles the MCU status LED, to blink at 2Hz
 * Rate: 4Hz
 * Priority: 1 (lowest)
 */
void vMCUStatusTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 250;		// In ticks (ms)
	
	// Task variables
	bool currentValue = 0;	// false
	
	// Setup MCU status pin as output
	pinMode(MCU_STATUS_PORT, MCU_STATUS_CH, IO_DIR_OUTPUT);
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Flip value
		currentValue = !currentValue;
		// Write out value
		setPin(MCU_STATUS_PORT, MCU_STATUS_CH, currentValue);
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod); 
	}
}


/* Heartbeat task
 * Sends node heartbeat out on the CANBus
 * Rate: 10Hz
 * Priority: 3
 */
void vHeartbeatTask(void *pvParameters) {
	// Function variables
	int i;
	
	// Get status variables
	MOB_STATUS *statuses = (MOB_STATUS*)pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1;		// In ticks (ms)
	
	// Data to send
	// HeartbeatFSM data;
	uint8_t counts[NO_MOBS];
	
	// Define CAN packet to send.
	// Format: {ID, Length, Data};
	static CAN_packet packet = {0x300, NO_MOBS, "\0\0\0\0\0\0"};
	
	// Setup values
	// data.currentState = RTD;
	// data.someData = 10;
	
	for(;;) {
		// Update counts
		for(i = 0; i < NO_MOBS; i++) {
			counts[i] = statuses[i].cnt;
		}
		
		// Copy counts to data array
		memcpy(packet.data, counts, NO_MOBS * sizeof(uint8_t));
		
		// Transmit the data. 
		// Format: (Packet Pointer, Mailbox, Timeout (in ticks));
		can_send(&packet, get_free_mob(), 100);
		
		// Delay 100ms
		vTaskDelayUntil(&xLastWakeTime, xPeriod);  
	}
}


/* Send over CAN
 * Uses send mailboxes to send enqueued messages over CAN
 * Currently unimplemented
 */
void vCANSendTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	
}


/* Receive from CAN
 * 
 * 
 * 
 */
void vCANReceiveTask(void *pvParameters) {
	// Function Variables
	volatile xQueueHandle queue;
	CAN_packet packet;
	uint8_t mob_num;
	volatile uint16_t param_val;
	
	// Get MOB number
	param_val = (uint16_t) pvParameters;
	mob_num = statuses[param_val].mob_num;
	
	// Make sure this is an RX mailbox
	// assert(MOB_DIRS[status.mob_num] == RX);
	
	// Critical section for queue creation
	taskENTER_CRITICAL();
	queue = xCANQueueCreate(MOB_IDS[mob_num], MOB_MASKS[mob_num], CAN_QUEUE_LEN, mob_num);
	taskEXIT_CRITICAL();
	
	// Check for failure
	if(queue == 0) {
		// Exit task
		vTaskDelete(NULL);
		return;
	}
	
	for(;;) {
		// Receive message
		xQueueReceive(queue, &packet, portMAX_DELAY);
		
		// Increment mailbox receive count
		statuses[param_val].cnt++;
		
		// See if a callback function is defined
		if(statuses[param_val].cbk != NULL) {
			// Call function with packet as parameter
			(*(statuses[param_val].cbk))(packet);
		}
	}
}


/* 
 * Tasks to add:
 *	- SW watchdog
 *	- 
 */