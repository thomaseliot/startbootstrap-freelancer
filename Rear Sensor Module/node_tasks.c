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

void vCoolingUpdateTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;

	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Period
	const TickType_t xPeriod = 1000/COOLING_UPDATE_TASK_RATE;		// In ticks (ms)

	// Initialize fan enable port
	pinMode(IO_PORTD, 1, IO_DIR_OUTPUT);

	//Pump ports
	pinMode(IO_PORTB, 3, IO_DIR_OUTPUT);
	pinMode(IO_PORTB, 4, IO_DIR_OUTPUT);
	
	//Fan PWM port
	pinMode(IO_PORTD, 3, IO_DIR_OUTPUT);

	// PWM Settings
	// Set at OCR0A, clear at TOP. Use Fast PWM Mode.
	pwmInit(PWM_0A, PWM_CLOCK_DIV1);
	
	// Enable fan
	setPin(IO_PORTD, 1, HIGH);
	

	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		switch(cooling_state){
			case FAN_OFF:
				pwmSetDutyCycle(PWM_0A, 0);
				setPin(IO_PORTB, 3, LOW);
				setPin(IO_PORTB, 4, LOW);
				leftPumpStatus = LOW;
				rightPumpStatus = LOW;
				break;
			case FAN_RAMP:
				pwmSetDutyCycle(PWM_0A, fan_duty);
				setPin(IO_PORTB, 3, HIGH);
				setPin(IO_PORTB, 4, HIGH);
				rightPumpStatus = HIGH;
				leftPumpStatus = HIGH;
				break;
			case FAN_ON:
				pwmSetDutyCycle(PWM_0A, 255);
				setPin(IO_PORTB, 3, HIGH);
				setPin(IO_PORTB, 4, HIGH);
				rightPumpStatus = HIGH;
				leftPumpStatus = HIGH;			
				break;
			default:
				pwmSetDutyCycle(PWM_0A, 0);
				setPin(IO_PORTB, 3, LOW);
				setPin(IO_PORTB, 4, LOW);		
				leftPumpStatus = LOW;
				rightPumpStatus = LOW;									
		}

		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

void vCoolingSetTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;

	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Period
	const TickType_t xPeriod = 1000/COOLING_SET_TASK_RATE;		// In ticks (ms)
	
	cooling_state = FAN_OFF;
	fan_duty = 0;
	
	//Calculate the step size to achieve the desired ramp time
	//Add for a ceiling, ensuring some ramp.
	unsigned int speed = (256/(COOLING_SET_TASK_RATE*RAMP_LENGTH)) + 1;
	vTaskDelayUntil(&xLastWakeTime, 2000);

	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		switch(cooling_state){
			case FAN_OFF:
				cooling_state = FAN_RAMP;
				break;
			case FAN_RAMP:
				if(255 - fan_duty < speed){
					// If fan_duty is within one step of the max,
					// set to max and change state
					fan_duty = 255;
					cooling_state = FAN_ON;
				}
				//Otherwise increment
				else fan_duty+=speed;
				break;
			case FAN_ON:
			default:
				break;
		}
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}


/* ADC sample task
 * Samples all ADC channels
 * Rate: 100Hz
 * Priority: 4 
 */
void vADCSampleTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
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
 * Sends node heartbeat out on the CANBus
 * Rate: 100Hz
 * Priority: 4
 */

void vHeartbeatTask(void *pvParameters) {
	// Function variables

	//Heartbeat message variable
	RSMHeartbeat_t RSMHeartbeat;
		
	// Get status variables
	MOB_STATUS *statuses = (MOB_STATUS*)pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000/ HEARTBEAT_TASK_RATE;		// In ticks (ms)
	
	// Define CAN packet to send.
	// Format: {ID, Length, Data};
	static CAN_packet packet;
	packet.id = RSM_HEARTBEAT_ID;
	packet.length = sizeof(RSMHeartbeat_t);		
	
	for(;;) {	
		// Data to send
		RSMHeartbeat.state = cooling_state;
		RSMHeartbeat.vbatt = adcVal(VBATT);
		RSMHeartbeat.fanCurrent = adcVal(FAN_IS);
		RSMHeartbeat.leftPumpStatus = leftPumpStatus;
		RSMHeartbeat.rightPumpStatus = rightPumpStatus;	
		
		//Copy data to message array
		memcpy(packet.data, &RSMHeartbeat, sizeof(AFCHeartbeat_t));
		
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
