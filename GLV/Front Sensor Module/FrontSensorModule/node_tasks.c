/*
 * tasks.c
 * 
 * Contains code for all tasks.
 *
 * Created: 11/23/2015 6:19:47 PM
 * Author: semerson
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "node_tasks.h"
#include "task.h"
#include "can_ids.h"
#include "cmr_64c1_lib.h"
#include "node_config.h"
#include "can_structs.h"


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
	config_io_pin(MCU_STATUS_PORT, MCU_STATUS_CH, IO_DIR_OUTPUT);
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Flip value
		currentValue = !currentValue;
		// Write out value
		set_io_pin(MCU_STATUS_PORT, MCU_STATUS_CH, currentValue);
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod); 
	}
}


/* Heartbeat task
 * Sends a node heartbeat out on the CANBus
 * Rate: 10Hz
 * Priority: 3
 */
void vHeartbeatTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000;		// In ticks (ms)
	
	// Data to send
	HeartbeatFSM data;
	
	// Define CAN packet to send. 
	// Format: {ID, Length, Data};
	static CAN_packet packet = {0x204, 2, "fa"};
	
	// Setup values
	data.currentState = RTD;
	data.someData = 10;
	
	for(;;) {
		// Transmit the data. 
		// Format: (Packet Pointer, Mailbox, Timeout (in ticks));
		can_send(&packet, 1, 100);
		
		// Delay 100ms
		vTaskDelayUntil(&xLastWakeTime, xPeriod);  
	}
}


/* Send over CAN
 * Uses send mailboxes to send enqueued messages over CAN
 * 
 * 
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
	// Make compiler happy
	(void) pvParameters;
	
	
}