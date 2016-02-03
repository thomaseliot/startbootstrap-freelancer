/*
 * FrontSensorModule.c
 *
 * Created: 11/23/2015 5:51:36 PM
 * Author : skir0
 */ 

#include <avr/io.h>
#include "node_tasks.h"
#include "cmr_64c1_lib.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"

#define mainMCU_STATUS_TASK_PRIORITY	1
#define mainHEARTBEAT_TASK_PRIORITY		2

int main(void)
{	
	// Initialize CAN with fixed baud rate, as defined in can_config.h
	can_init(1);
		
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

