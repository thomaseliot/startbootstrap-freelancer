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
#include "node_config.h"
#include "assert.h"


/************************************************************************/
/* TO BE PLACED IN WDT FILE --- IN PROGRESS                             */
/************************************************************************

#define 

// Watchdog arrays for all tasks
static  {
	
}

/* 

/* kickExtWDT
 * Kick the external watchdog timer
 *
void kickWDT() {
	
}

/************************************************************************/
/*                                                                      */
/************************************************************************/

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
		// Loop through and sample all thermistors
		for(i = THERM1; i <= THERM8; i++) {
			// Update ADC value in struct
			updateADC(i);
		}
		
		// Delay until next period
   		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

/* Watchdog kick task
 * Checks to make sure all tasks have run, and kicks the external watchdog
 * Rate: 2Hz
 * Priority: 5
 */
void vWatchdogTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Function variables
	int i;
	
	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / WATCHDOG_TASK_RATE;		// In ticks (ms)
	
	// Set watchdog kick as output
	pinMode(EXT_WD_KICK_PORT, EXT_WD_KICK_CH, IO_DIR_OUTPUT);
	setPin(EXT_WD_KICK_PORT, EXT_WD_KICK_CH, LOW);
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Kick the watchdog
		setPin(EXT_WD_KICK_PORT, EXT_WD_KICK_CH, HIGH);
		
		// Wait a little bit
		for(i = 0; i < 100; i++) {
			asm("NOP");
		}
		
		// Set back to low
		setPin(EXT_WD_KICK_PORT, EXT_WD_KICK_CH, LOW);
		
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

/* Thermistor Read Task
 * Reads the thermistor values and reports the max value
 * Rate: 100Hz
 * Priority: 3
 */
void vThermistorReadTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Task variables
	uint8_t i;
	uint16_t thisVal, maxVal = 0;
	
	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / THERMISTOR_READ_TASK_RATE;		// In ticks (ms)
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Find maximum thermistor value
		for(i = THERM1; i <= THERM8; i++) {
			// Get current value
			thisVal = adcVal(i);
			// Update maxval if greater
			if(thisVal > maxVal) maxVal = thisVal;
		}
		
		// Update output thermistor value
		maxThermistorValue = maxVal;
		
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

/* Thermistor Write Task
 * Writes out the thermistor value to the digital pots
 * Rate: 100Hz
 * Priority: 3
 */
void vThermistorWriteTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Function variables
	volatile uint8_t valueToWrite = 0x00;
	volatile uint8_t upperByte, lowerByte;
	volatile uint8_t readUpper, readLower;
	volatile uint8_t command;
	SPISlave pot1 = spiSlaves[POT1_SS];
	SPISlave pot2 = spiSlaves[POT2_SS];
	
	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / THERMISTOR_WRITE_TASK_RATE;		// In ticks (ms)
	
	// Write some zeros for good measure
	//taskENTER_CRITICAL();
	//spiSelect(pot1);
	spiWrite(0x00);
	spiWrite(0x00);
	//spiDeselect(pot1);
	//spiSelect(pot2);
	//spiWrite(0x00);
	//spiWrite(0x00);
	//spiDeselect(pot2);
	//taskEXIT_CRITICAL();
	
	// Enable RDAC register write access for each pot
	upperByte = 0x1C;
	lowerByte = 0x03;
	taskENTER_CRITICAL();
	spiSelect(pot1);
	spiWrite(0x20);
	spiWrite(0x00);
	readUpper = spiRead();
	readLower = spiRead();
	spiWrite(0x1C);
    spiWrite(0x03);
	
	spiDeselect(pot1);
	
	//spiWrite(0x00);
	//spiWrite(0x00);
	
	//spiSelect(pot1);
	//spiWrite(0x1C);
	//spiWrite(0x02);
	//spiDeselect(pot1);
	taskEXIT_CRITICAL();
	
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Setup bytes to write
		command = 0x01;
		upperByte = (command << 2) | (valueToWrite >> 6);
		lowerByte = valueToWrite << 2;
		
		// Write
		taskENTER_CRITICAL();
		spiSelect(pot1);
		spiWrite(upperByte);
		spiWrite(lowerByte);
		spiDeselect(pot1);
		taskEXIT_CRITICAL();

		valueToWrite += 1;
		
		
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}