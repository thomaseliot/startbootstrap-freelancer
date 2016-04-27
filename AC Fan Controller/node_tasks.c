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
/*
void vFanTestTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;

	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Period
	const TickType_t xPeriod = FAN_TEST_TASK_PERIOD;		// In ticks (ms)

	// Initialize IO ports
	pinMode(IO_PORTC, 0, IO_DIR_OUTPUT);
	pinMode(IO_PORTD, 0, IO_DIR_OUTPUT);
	pinMode(IO_PORTD, 1, IO_DIR_OUTPUT);

	// All On
	pinMode(IO_PORTD, 3, IO_DIR_OUTPUT);

	// PWM Settings
	// Set at OCR0A, clear at TOP. Use Fast PWM Mode. Clock Prescaler of 8.
	//TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);
	//CCR0B |= (1 << CS00);

	// Task variables
	uint8_t fanState = 0;
	uint8_t testPWMs[8] = {63, 127, 191, 255, 191, 127, 63, 0};
	//uint8_t testPWMs[8] = {31,  63,  95, 127,  95,  63, 31, 0};

	// Enable three fans
	setPin(IO_PORTC, 0, HIGH);
	setPin(IO_PORTD, 0, HIGH);
	setPin(IO_PORTD, 1, HIGH);
	

	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		
		// Load the PWM duty cycle and move to the next index, with wrap-around
		//OCR0A = testPWMs[(fanState++) & (0x07)];
		setPin(IO_PORTD, 3, HIGH);

		//taskYIELD();
		vTaskDelayUntil(&xLastWakeTime, xPeriod);

	}
	
	// Delay until next period
}
*/

void vFanUpdateTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;

	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Period
	const TickType_t xPeriod = 1000/FAN_UPDATE_TASK_RATE;		// In ticks (ms)

	// Initialize IO ports
	pinMode(IO_PORTC, 0, IO_DIR_OUTPUT);
	pinMode(IO_PORTD, 0, IO_DIR_OUTPUT);
	pinMode(IO_PORTD, 1, IO_DIR_OUTPUT);
	
	pinMode(IO_PORTD, 3, IO_DIR_OUTPUT);

	// PWM Settings
	// Set at OCR0A, clear at TOP. Use Fast PWM Mode.
	//TCCR0A |= (1<<WGM01)|(1<<WGM00)|(1<<COM0A1);
	//TCCR0B |= (1 << CS00);
	pwmInit(PWM_0A, PWM_CLOCK_DIV1);
	
	// Enable three fans
	setPin(IO_PORTC, 0, HIGH);
	setPin(IO_PORTD, 0, HIGH);
	setPin(IO_PORTD, 1, HIGH);
	

	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		switch(fan_state){
			case FAN_OFF:
				pwmSetDutyCycle(PWM_0A, 0);
				break;
			case FAN_RAMP:
				pwmSetDutyCycle(PWM_0A, fan_duty);
				break;
			case FAN_ON:
				pwmSetDutyCycle(PWM_0A, MAX_DUTY);			
				break;
			default:
				pwmSetDutyCycle(PWM_0A, 0);			
		}

		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

void vFanSetTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;

	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Period
	const TickType_t xPeriod = 1000/FAN_SET_TASK_RATE;		// In ticks (ms)
	
	fan_state = FAN_OFF;
	fan_duty = 0;
	
	//Calculate the step size to achieve the desired ramp time
	//Add for a ceiling, ensuring some ramp.
	unsigned int speed = (256/(FAN_SET_TASK_RATE*RAMP_LENGTH)) + 1;
	vTaskDelayUntil(&xLastWakeTime, 2000);

	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		switch(fan_state){
			case FAN_OFF:
				fan_state = FAN_RAMP;
				break;
			case FAN_RAMP:
				if(MAX_DUTY - fan_duty < speed){
					// If fan_duty is within one step of the max,
					// set to max and change state
					fan_duty = MAX_DUTY;
					fan_state = FAN_ON;
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
	AFCHeartbeat_t AFCHeartbeat;
		
	// Get status variables
	MOB_STATUS *statuses = (MOB_STATUS*)pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000/ HEARTBEAT_TASK_RATE;		// In ticks (ms)
	


	
	// Define CAN packet to send.
	// Format: {ID, Length, Data};
	static CAN_packet packet;
	packet.id = 0x205;
	packet.length = sizeof(AFCHeartbeat_t);		
	
	for(;;) {	
		// Data to send
		AFCHeartbeat.state = fan_state;
		AFCHeartbeat.vbatt = adcVal(VBATT);
		AFCHeartbeat.fan1Current = adcVal(FAN1_IS);
		AFCHeartbeat.fan2Current = adcVal(FAN2_IS);
		AFCHeartbeat.fan3Current = adcVal(FAN3_IS);	
		
		//Copy data to message array
		memcpy(packet.data, &AFCHeartbeat, sizeof(AFCHeartbeat_t));
		
		// Transmit the data. 
		// Format: (Packet Pointer, Mailbox, Timeout (in ticks));
		//int mob = get_free_mob();
		can_send(&packet, 3, 100);
		
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
