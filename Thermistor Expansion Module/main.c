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


// Main function, runs once
int main(void)
{	
	/* Initialization */
	
	// Initialize ADC
	initADC();
	
	// Initialize SPI
	spiInit();
	spiSetClockDivider(SPI_CLOCK_DIV32);
	spiSetBitOrder(SPI_MSBFIRST);
	spiSetDataMode(SPI_MODE1);

	
	// Hold both thermistor out relays open
	pinMode(OPEN_FAULT_PORT, OPEN_FAULT_CH, IO_DIR_OUTPUT);
	pinMode(SHORT_FAULT_PORT, SHORT_FAULT_CH, IO_DIR_OUTPUT);
	setPin(OPEN_FAULT_PORT, OPEN_FAULT_CH, HIGH);
	setPin(SHORT_FAULT_PORT, SHORT_FAULT_CH, HIGH);
	
	// Enable second parallel pot
	pinMode(POT2_CTRL_PORT, POT2_CTRL_CH, IO_DIR_OUTPUT);
	setPin(POT2_CTRL_PORT, POT2_CTRL_CH, HIGH);
	
	
	/* Tasks */
	
	// MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, MCU_STATUS_TASK_PRIORITY, NULL);
	
	// ADC read task
	// Rate: 100Hz
	xTaskCreate(vADCSampleTask, "ADC", configMINIMAL_STACK_SIZE,
		NULL, ADC_SAMPLE_TASK_PRIORITY, NULL);
		
	// Thermistor read task
	//xTaskCreate(vThermistorReadTask, "TEMPIN", configMINIMAL_STACK_SIZE,
	//	NULL, THERMISTOR_READ_TASK_PRIORITY, NULL);
	
	// Thermistor write task
	xTaskCreate(vThermistorWriteTask, "TEMPOUT", configMINIMAL_STACK_SIZE,
		NULL, THERMISTOR_WRITE_TASK_PRIORITY, NULL);
		
	// Watchdog kick task
	xTaskCreate(vWatchdogTask, "EXTWDT", configMINIMAL_STACK_SIZE,
		NULL, WATCHDOG_TASK_PRIORITY, NULL);
	
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler takes over from here
	return 0;
}

