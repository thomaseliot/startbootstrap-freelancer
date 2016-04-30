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
#include "can_payloads.h"

//File globals
static uint8_t fan_duty;
static FanState fanTarget_state, fan1_state, fan2_state, fan3_state;
static NodeState currentState = GLV_ON;


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
	pwmInit(PWM_0A, PWM_CLOCK_DIV1);
	
	// Enable three fans
	setPin(IO_PORTC, 0, HIGH);
	setPin(IO_PORTD, 0, HIGH);
	setPin(IO_PORTD, 1, HIGH);
	

	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		
			switch(fanTarget_state){
				case FAN_ERROR:
				case FAN_OFF:
					pwmSetDutyCycle(PWM_0A, 0);
					break;
				case FAN_RAMPING:
				case FAN_ON:
					pwmSetDutyCycle(PWM_0A, fan_duty);
					break;
				default:
					pwmSetDutyCycle(PWM_0A, 0);			
			}
			
			//pwmSetDutyCycle(PWM_0A, fan_duty);


		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}

void vFanSetStateTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;

	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();

	// Period
	const TickType_t xPeriod = 1000/FAN_SET_STATE_TASK_RATE;		// In ticks (ms)
	
	fanTarget_state = FAN_OFF;
	fan_duty = 0;
	
	//Calculate the step size to achieve the desired ramp time
	//Add for a ceiling, ensuring some ramp.
	unsigned int speed = (256/(FAN_SET_STATE_TASK_RATE*RAMP_LENGTH)) + 1;
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		currentState = SMHeartbeat.state;
		switch(currentState){
			case GLV_ON:
				switch(SMHeartbeat.targetState){
					case GLV_ON:
						fanTarget_state = FAN_OFF;
						break;
					case HV_EN:
					case RTD:
						fanTarget_state = FAN_ON;
					default:
						break;
				}
				break;
			case HV_EN:
			case RTD:
				switch(SMHeartbeat.targetState){
					case HV_EN:
					case RTD:
						fanTarget_state = FAN_ON;
						if(adcVal(FAN1_IS) == 0) {
							fan1_state = FAN_ERROR;
						}
						if(adcVal(FAN2_IS) == 0) {
							fan2_state = FAN_ERROR;
						}
						if(adcVal(FAN3_IS) == 0) {
							fan3_state = FAN_ERROR;
						}
						break;
					case GLV_ON:
					default:
						fanTarget_state = FAN_OFF;
						break;
				}
				break;
			case ERROR:
				fanTarget_state = FAN_ERROR;
			default:
				fanTarget_state = FAN_OFF;
				break;
		}
		
		switch(fanTarget_state){
			case FAN_RAMPING:
			case FAN_ON:
				if(MAX_DUTY - fan_duty <= speed){
					// If fan_duty is within one step of the max,
					// set to max and change state
					fan_duty = MAX_DUTY;
					fan1_state = FAN_ON;
					fan2_state = FAN_ON;
					fan3_state = FAN_ON;
								
				}
				//Otherwise increment
				else { 
					fan_duty+=speed;
					fan1_state = FAN_RAMPING;
					fan2_state = FAN_RAMPING;
					fan3_state = FAN_RAMPING;
				}
				break;
			case FAN_ERROR:
				fan1_state = FAN_ERROR;
				fan2_state = FAN_ERROR;
				fan3_state = FAN_ERROR;
				fan_duty = 0;
				break;

			case FAN_OFF:
			default:
				fan1_state = FAN_OFF;
				fan2_state = FAN_OFF;
				fan3_state = FAN_OFF;
				fan_duty = 0;
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

void vTempSampleTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Previous wake time pointer, initialized to current tick count.
	// This gets updated by vTaskDelayUntil every time it is called
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / TEMP_SAMPLE_TASK_RATE;		// In ticks (ms)
	
	// Task variables
	int16_t temp_var;
	SPISlave temp = spiSlaves[TEMP];

	
	taskENTER_CRITICAL();
	spiSelect(temp);
	spiWrite(0x00);
	spiWrite(0x00);
	spiDeselect(temp);
	taskEXIT_CRITICAL();
	
	// Executes infinitely with defined period using vTaskDelayUntil
	for(;;) {
		// Sample all ADC channels
		taskENTER_CRITICAL();
		spiSelect(temp);
		temp_var = SPI_read();
		spiDeselect(temp);
		sysTemp = temp_var >> 2;
		taskEXIT_CRITICAL();
		
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
	//AFCHeartbeat_t AFCHeartbeat;
		
	// Get status variables
	MOB_STATUS *statuses = (MOB_STATUS*)pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000/ HEARTBEAT_TASK_RATE;		// In ticks (ms)
	


	
	// Define CAN packet to send.
	// Format: {ID, Length, Data};
	static CAN_packet packet;
	packet.id = AFC_HEARTBEAT_ID;
	packet.length = sizeof(AFCHeartbeat_t);		
	
	for(;;) {	
		// Data to send
		AFCHeartbeat.state = currentState;
		AFCHeartbeat.vbatt = adcVal(VBATT);
		AFCHeartbeat.fan1Current = adcVal(FAN1_IS);
		AFCHeartbeat.fan1Status  = fan1_state;
		AFCHeartbeat.fan2Current = adcVal(FAN2_IS);
		AFCHeartbeat.fan2Status  = fan2_state;
		AFCHeartbeat.fan3Current = adcVal(FAN3_IS);	
		AFCHeartbeat.fan3Status  = fan3_state;
		
		//Copy data to message array
		memcpy(packet.data, &AFCHeartbeat, sizeof(AFCHeartbeat_t));
		
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
		
		// See if a callback function is defined
		if(status.cbk != NULL) {
			// Call function with packet as parameter
			(*(status.cbk))(packet);
		}
	}
}


/* Check for receive message timeouts
 * 
 */

void vCANTimeoutMonitorTask(void *pvParameters) {
	// Make compiler happy
	(void) pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / TIMEOUT_MONITOR_TASK_RATE;		// In ticks (ms)
	
	for(;;) {
		
		/************************************************************************/
		/* Driver Interface Module                                              */
		/************************************************************************/
		
		// Driver Interface Module Stale Check
		if(SMHeartbeatReceiveMeta.staleFlag && !SMHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment if stale and not timed out
			SMHeartbeatReceiveMeta.missCount++;
			} else if (!SMHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			SMHeartbeatReceiveMeta.missCount = 0;
			SMHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Driver Interface Module State Check
		if(SMHeartbeat.state != currentState && !SMHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			SMHeartbeatReceiveMeta.differentStateCount++;
			} else if(SMHeartbeat.state == currentState) {
			// Reset miss count if state matches
			SMHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Driver Interface Module Timeout
		if(SMHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			SMHeartbeatReceiveMeta.timeoutFlag = 1;
			} else {
			SMHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Driver Interface Module State Timeout
		if(SMHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			SMHeartbeatReceiveMeta.wrongStateFlag = 1;
			} else {
			// Reset timeout flag if not timed out
			SMHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
				
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);
	}
}
