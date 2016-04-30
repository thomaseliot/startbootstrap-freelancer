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
#include "can_callbacks.h"
#include "can_payloads.h"
#include "can_config.h"

// Global status variable
MOB_STATUS statuses[NO_MOBS];

// Main function, runs once
int main(void)
{	
	// Function variables	
	/* Initialization */
	
	// Initialize ADC
	initADC();
	//Initialize CAN
	can_init();
	initPayloads();	// Initialize SPI
	
	//spiInit();
	/*
	spiSetClockDivider(SPI_CLOCK_DIV16);
	spiSetBitOrder(SPI_MSBFIRST);
	spiSetDataMode(SPI_MODE0);
	*/
	uint16_t i;
	
	for(i = 0; i < NO_MOBS; i++) {
			
		// Initialize mailbox status
		statuses[i].mob_num = i;
		statuses[i].cnt = 0;
			
		if(MOB_DIRS[i] == RX) {
			// Assign callback to task
			statuses[i].cbk = MOB_CALLBACKS[i];
				
			// Name of task
			char name[4];
			sprintf(name, "RCV%d", i);
			// Create task for this mailbox
			xTaskCreate(vCANReceiveTask, name, configMINIMAL_STACK_SIZE,
			(void *)(&statuses[i]), MOB_PRIORITIES[i], NULL);
		}
	}
		
	// MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, MCU_STATUS_TASK_PRIORITY, NULL);
	
	// Heartbeat task, to heartbeat on CAN
	// Rate: 10Hz
	xTaskCreate(vHeartbeatTask, "HEART", configMINIMAL_STACK_SIZE,
		(void *)statuses, HEARTBEAT_TASK_PRIORITY, NULL);
		
	
	// ADC read task
	// Rate: 100Hz
	xTaskCreate(vADCSampleTask, "ADC", configMINIMAL_STACK_SIZE,
		NULL, ADC_SAMPLE_TASK_PRIORITY, NULL);
		
	// Module-specific tasks here
	
	//Temp read task
	//Rate: 100Hz
	//xTaskCreate(vTempSampleTask, "TEMP", configMINIMAL_STACK_SIZE,
	//NULL, ADC_SAMPLE_TASK_PRIORITY, NULL);
	
	// Fan speed update task
	// Rate: 50Hz
	xTaskCreate(vFanSetStateTask, "FAN_ST", configMINIMAL_STACK_SIZE,
		NULL, FAN_SET_STATE_TASK_PRIORITY, NULL);
		
	// Fan speed update task
	// Rate: 100Hz	
	xTaskCreate(vFanUpdateTask, "FAN_UP", configMINIMAL_STACK_SIZE,
		NULL, FAN_UPDATE_TASK_PRIORITY, NULL);
		

	//xTaskCreate(vCANTimeoutMonitorTask, "CANMON", configMINIMAL_STACK_SIZE,
	//	NULL, TIMEOUT_MONITOR_TASK_PRIORITY, NULL);
			
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler takes over from here
	return 0;
}