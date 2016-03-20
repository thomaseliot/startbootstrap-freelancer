/* main.c
 * 
 * The main function, runs once then the scheduler takes over
 * 
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 

#include <avr/io.h>
#include <stdio.h>

/* Scheduler include files */
#include "FreeRTOS.h"
#include "task.h"

/* Node configuration */
#include "node_config.h"

/* Task include files */
#include "node_tasks.h"

/* Library and driver include files */
#include "cmr_64c1_lib.h"
#include "adc.h"
#include "spi.h"

// Global status variable
MOB_STATUS statuses[NO_MOBS];

// Main function, runs once
int main(void)
{	
	// Function variables
	uint16_t i;

	/* Initialization */
	
	// Initialize ADC
	initADC();
	
	// Initialize SPI
	spiInit();
	// TODO: Change these for SPI temp sensor
	spiSetClockDivider(SPI_CLOCK_DIV64);
	spiSetBitOrder(SPI_MSBFIRST);
	spiSetDataMode(SPI_MODE1);
	
	
	// Assign mailbox callbacks
	statuses[0].cbk = mb1_callback;
	statuses[1].cbk = mb2_callback;
	statuses[2].cbk = mb3_callback;
	statuses[3].cbk = mb4_callback;
	
	/* Tasks */
	
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
	
	// MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, MCU_STATUS_TASK_PRIORITY, NULL);
	
	// ADC read task
	// Rate: 100Hz
	xTaskCreate(vADCSampleTask, "ADC", configMINIMAL_STACK_SIZE,
		NULL, ADC_SAMPLE_TASK_PRIORITY, NULL);
		
	// Module-specific tasks here
	
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler takes over from here
	return 0;
}