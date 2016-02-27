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
	
	// Create the MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, mainMCU_STATUS_TASK_PRIORITY, NULL);
	
	// Create ADC read task
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler handles the rest
	return 0;
}

