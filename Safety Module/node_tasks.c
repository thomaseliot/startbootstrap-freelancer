/* node_tasks.c
 * Contains code for all tasks
 * 
 * Copyright (c) 2016, Carnegie Mellon Racing
 */ 

#include <avr/io.h>
#include <string.h>
#include <stdbool.h>
#include "node_tasks.h"
#include "queue.h"
#include "cmr_64c1_lib.h"
#include "can_ids.h"
#include "can_structs.h"
#include "node_config.h"
#include "cmr_constants.h"
#include "assert.h"

// Current node state, initialized to GLV_ON
uint8_t currentState = GLV_ON;

// Target global state, used for state transitioning
uint8_t targetState = GLV_ON;	// Initialized to GLV_ON

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
	const TickType_t xPeriod = 1000 / MCU_STATUS_TASK_RATE;		// In ticks (ms)
	
	// Task variables
	bool currentValue = false;	// false
	
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


/* ADC sample task
 * Samples all ADC channels
 * Rate: 100Hz
 * Priority: 5 (highest)
 */
void vADCSampleTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	volatile uint16_t j;
	
	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();
		
	// Period
	const TickType_t xPeriod = 1000 / ADC_SAMPLE_TASK_RATE;		// In ticks (ms)
	
	// Task variables
	unsigned i;
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Sample all ADC channels
		for(i = LOWEST_ADC_CH; i <= HIGHEST_ADC_CH; i++) {
			// Update ADC value in struct
			updateADC(i);
		}
		
		// Delay until next period
   		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

/* Heartbeat task
 * Sends node heartbeat out on CAN
 * Rate: 100Hz
 * Priority: 3
 */
void vHeartbeatTask(void *pvParameters) {
	// Function variables
	int i;
	
	// Heartbeat message variable
	SMHeartbeat_t SMHeartbeat;
	
	// Get status variables
	MOB_STATUS *statuses = (MOB_STATUS*)pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / HEARTBEAT_TASK_RATE;		// In ticks (ms)
	
	// Initialize heartbeat message
	SMHeartbeat.state = currentState;
	SMHeartbeat.targetState = targetState;
	SMHeartbeat.vbatt = adcVal(VBATT);
	
	// Define CAN packet to send
	static CAN_packet packet;
	packet.id = SM_HEARTBEAT_ID;
	packet.length = sizeof(SMHeartbeat_t);
	
	for(;;) {
		// Update vbatt value
		SMHeartbeat.vbatt = adcVal(VBATT);
		
		// Copy heartbeat to message array
		memcpy(packet.data, &SMHeartbeat, sizeof(struct SMHeartbeat_t));
		
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
/*
void vCANSendTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	
}
*/


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
	// Get status variables
	MOB_STATUS status = *((MOB_STATUS *)pvParameters);
	
	mob_num = status.mob_num;
	
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
		status.cnt++;
		
		// See if a callback function is defined
		if(status.cbk != NULL) {
			// Call function with packet as parameter
			(*(status.cbk))(packet);
		}
	}
}