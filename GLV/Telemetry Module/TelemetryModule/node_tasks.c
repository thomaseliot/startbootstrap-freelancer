/*
 * tasks.c
 *
 * Created: 11/23/2015 6:19:47 PM
 *  Author: skir0
 */ 

#include <avr/io.h>
#include <stdbool.h>
#include "node_tasks.h"
#include "task.h"
#include "cmr_64c1_lib.h"
#include "can_ids.h"
#include "can_structs.h"
#include "node_config.h"

// Globals, to be removed later
st_cmd_t heartbeat_cmd_st; // Receive command for 1 reserved heartbeat mailbox
uint64_t can_send_buffer;	

/* MCU Status task
 * Toggles the MCU status LED, to blink at 2Hz
 * Rate: 4Hz
 * Priority: 1 (lowest)
 */
void vMCUStatusTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	// Task variables
	Bool currentValue = 0;	// false
	// Setup MCU status pin as output
	config_io_pin(MCU_STATUS_PORT, MCU_STATUS_CH, IO_DIR_OUTPUT);
	
	//set_io_pin(IO_PORT_D, 5, 1);
	
	config_io_pin(IO_PORT_D, 5, IO_DIR_INPUT);
	config_io_pin(IO_PORT_D, 6, IO_DIR_OUTPUT);
	
	set_io_pin(IO_PORT_D, 6, LOW);
		
	for(;;) {
		// Flip value
		currentValue = !currentValue;
						
		// Write out value
		set_io_pin(MCU_STATUS_PORT, MCU_STATUS_CH, currentValue);
		// Delay 250ms for 4Hz
		vTaskDelay((TickType_t)1000); 
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
	// Data to send
	HeartbeatFSM data;
	
	// Setup values
	data.currentState = RTD;
	data.someData = 10;
	
	for(;;) {
		// Transmit the data
		can_tx(FSM_HEARTBEAT_ID, (uint8_t *)&data, sizeof(HeartbeatFSM));
		// Delay 100ms
		vTaskDelay((TickType_t)100); 
	}
}

/* Send over CAN
 * 
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