/* main.c
 * 
 * The main function, runs once then the scheduler takes over
 */ 

#include <avr/io.h>
#include <stdio.h>

/* Scheduler include files */
#include "FreeRTOS.h"
#include "task.h"

#include "cmr_64c1_lib.h"
#include "node_config.h"

/* Task include files */
#include "node_tasks.h"

/* Driver include files */
#include "adc.h"
#include "spi.h"


// Main function, runs once
int main(void)
{	
	// Initialize ADC
	initAdc();
	
	// Initialize SPI
	spiInit();
	spiSetClockDivider(SPI_CLOCK_DIV64);
	spiSetBitOrder(SPI_MSBFIRST);
	spiSetDataMode(SPI_MODE1);
	
	// Hold both thermistor out relays open
	config_io_pin(OPEN_FAULT_PORT, OPEN_FAULT_CH, IO_DIR_OUTPUT);
	config_io_pin(SHORT_FAULT_PORT, SHORT_FAULT_CH, IO_DIR_OUTPUT);
	set_io_pin(OPEN_FAULT_PORT, OPEN_FAULT_CH, HIGH);
	set_io_pin(SHORT_FAULT_PORT, SHORT_FAULT_CH, LOW);
	
	// Create the MCU status task, to blink the LED
	// Rate: 4Hz
    xTaskCreate(vMCUStatusTask, "STATUS", configMINIMAL_STACK_SIZE, 
		NULL, MCU_STATUS_TASK_PRIORITY, NULL);
	
	// Create ADC read task
	// Rate: 100Hz
	xTaskCreate(vADCSampleTask, "ADC", configMINIMAL_STACK_SIZE,
		NULL, ADC_SAMPLE_TASK_PRIORITY, NULL);
		
	// Create thermistor read task
	//xTaskCreate(vThermistorReadTask, "TEMPIN", configMINIMAL_STACK_SIZE,
	//	NULL, THERMISTOR_READ_TASK_PRIORITY, NULL);
	
	// Create thermistor write task
	xTaskCreate(vThermistorWriteTask, "TEMPOUT", configMINIMAL_STACK_SIZE,
		NULL, THERMISTOR_WRITE_TASK_PRIORITY, NULL);
		
	// Create watchdog kick task
	xTaskCreate(vWatchdogTask, "EXTWDT", configMINIMAL_STACK_SIZE,
		NULL, WATCHDOG_TASK_PRIORITY, NULL);
	
	// Start the scheduler
	vTaskStartScheduler();
	
	// Return, the scheduler handles the rest
	return 0;
}

