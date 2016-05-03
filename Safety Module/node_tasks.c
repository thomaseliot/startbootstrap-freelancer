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
#include "can_ids.h"
#include "can_structs.h"
#include "can_callbacks.h"
#include "can_payloads.h"
#include "node_config.h"
#include "assert.h"

// Current and target state variable storage
static NodeState currentState = GLV_ON;
static NodeState targetState = GLV_ON;

/* Initialize tasks
 */
void initTasks() {
	// Initialize state variables
	currentState = GLV_ON;
	targetState = GLV_ON;
}

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
 * Sends node heartbeat out on CAN
 * Rate: 100Hz
 * Priority: 3
 */
void vHeartbeatTask(void *pvParameters) {
	// Function variables
	int i;
	
	// Get status variables
	MOB_STATUS *statuses = (MOB_STATUS*)pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / HEARTBEAT_TASK_RATE;		// In ticks (ms)
	
	// Initialize heartbeat message
	SMHeartbeat.state = currentState;
	SMHeartbeat.targetState = targetState;
	SMHeartbeat.vbatt = adcVal(VBATT);
	SMHeartbeat.nodeHeartbeatStatus = CCHeartbeatReceiveMeta.timeoutFlag
		| (FSMHeartbeatReceiveMeta.timeoutFlag << 1)
		| (RSMHeartbeatReceiveMeta.timeoutFlag << 2)
		| (DIMHeartbeatReceiveMeta.timeoutFlag << 3)
		| (AFCHeartbeatReceiveMeta.timeoutFlag << 4)
		| (TMHeartbeatReceiveMeta.timeoutFlag << 5);
	SMHeartbeat.nodeStateStatus = CCHeartbeatReceiveMeta.wrongStateFlag
		| (FSMHeartbeatReceiveMeta.wrongStateFlag << 1)
		| (RSMHeartbeatReceiveMeta.wrongStateFlag << 2)
		| (DIMHeartbeatReceiveMeta.wrongStateFlag << 3)
		| (AFCHeartbeatReceiveMeta.wrongStateFlag << 4)
		| (TMHeartbeatReceiveMeta.wrongStateFlag << 5);
	
	// Define CAN packet to send
	CAN_packet packet;
	packet.id = SM_HEARTBEAT_ID;
	packet.length = sizeof(SMHeartbeat_t);
	
	for(;;) {
		// Update fields
		SMHeartbeat.state = currentState;
		SMHeartbeat.targetState = targetState;
		SMHeartbeat.vbatt = adcVal(VBATT);
		SMHeartbeat.nodeHeartbeatStatus = CCHeartbeatReceiveMeta.timeoutFlag
			| (FSMHeartbeatReceiveMeta.timeoutFlag << 1)
			| (RSMHeartbeatReceiveMeta.timeoutFlag << 2)
			| (DIMHeartbeatReceiveMeta.timeoutFlag << 3)
			| (AFCHeartbeatReceiveMeta.timeoutFlag << 4)
			| (TMHeartbeatReceiveMeta.timeoutFlag << 5);
		SMHeartbeat.nodeStateStatus = CCHeartbeatReceiveMeta.wrongStateFlag
		| (FSMHeartbeatReceiveMeta.wrongStateFlag << 1)
		| (RSMHeartbeatReceiveMeta.wrongStateFlag << 2)
		| (DIMHeartbeatReceiveMeta.wrongStateFlag << 3)
		| (AFCHeartbeatReceiveMeta.wrongStateFlag << 4)
		| (TMHeartbeatReceiveMeta.wrongStateFlag << 5);
		
		// Copy heartbeat to message array
		memcpy(packet.data, &SMHeartbeat, sizeof(SMHeartbeat_t));
		
		// Transmit the data
		// Format: (Packet Pointer, Mailbox, Timeout (in ticks));
		can_send(&packet, get_free_mob(), 100);
		
		// Delay 10ms
		vTaskDelayUntil(&xLastWakeTime, xPeriod);  
	}
}

/* CAN State Sequencing Task
 * Manages state of the whole system
 * Rate: 100Hz
 * Priority: 4
 */
void vCANStateSequenceTask(void *pvParameters) {	
	// Function variables
	NodeState nextState;
	
	// Make compiler happy
	(void) pvParameters;
	
	// Previous wake time pointer
	TickType_t xLastWakeTime = xTaskGetTickCount();
	
	// Period
	const TickType_t xPeriod = 1000 / STATE_SEQUENCE_TASK_RATE;		// In ticks (ms)
	
	for(;;) {
		// Update requested state from driver interface
		if(!DIMHeartbeatReceiveMeta.timeoutFlag 
			&& ((abs(DIMHeartbeat.requestedState - currentState) <= 1))					// Allow any adjacent state
			|| DIMHeartbeat.requestedState == GLV_ON && currentState == ERROR			// Allow ERROR -> GLV_ON transition
			|| DIMHeartbeat.requestedState == GLV_ON && currentState == CLEAR_ERROR)	// Allow CLEAR_ERROR -> GLV_ON transition
		{
			// If valid state requested, update target state
			targetState = DIMHeartbeat.requestedState;
		} 
		else {
			// If invalid, target the current state
			targetState = currentState;
		}
		
		// No state transition by default
		nextState = currentState;
		
		// Error transitions
		// If any critical heartbeats timed out, go to error state
		if(AFCHeartbeatReceiveMeta.timeoutFlag
			// || RSMHeartbeatReceiveMeta.timeoutFlag				// broken right now, uncomment later
			|| FSMHeartbeatReceiveMeta.timeoutFlag
			|| DIMHeartbeatReceiveMeta.timeoutFlag
			|| CCHeartbeatReceiveMeta.timeoutFlag) 
		{
			// Transition to error state
			nextState = ERROR;
		}
		
		// If any nodes are in the error state, go to the error state
		if((AFCHeartbeat.state == ERROR
			// || RSMHeartbeat.state == ERROR						// broken right now, uncomment later
			|| FSMHeartbeat.state == ERROR
			|| DIMHeartbeat.state == ERROR
			|| CCHeartbeat.state == ERROR)
			&& currentState != CLEAR_ERROR) 
		{
			nextState = ERROR;
		}
		
		// Current state actions
		switch(currentState) {
			case GLV_ON:
				// State actions
				
				// State transitions
				// GLV_ON -> HV_EN
				if(targetState == HV_EN && nextState != ERROR) {
					if(CCHeartbeat.state == currentState						// Make sure everything is in GLV_ON
						&& FSMHeartbeat.state == currentState
						// && RSMHeartbeat.state == currentState
						&& AFCHeartbeat.state == currentState
						// && DIMHeartbeat.state == currentState
						&& AFCHeartbeat.fan1Status == FAN_LOW					// Make sure all fans/pumps turned on
						&& AFCHeartbeat.fan2Status == FAN_LOW
						&& AFCHeartbeat.fan3Status == FAN_LOW
						// && RSMHeartbeat.radiatorFanStatus == FAN_ON
						// && RSMHeartbeat.leftPumpStatus == PUMP_ON
						// && RSMHeartbeat.rightPumpStatus == PUMP_ON
						)
					{
						nextState = HV_EN;
					}
				}
				break;
				
			case HV_EN:
				// State actions
			
				// State transitions
				// HV_EN -> ERROR
				if(AFCHeartbeat.fan1Status == FAN_ERROR							// Transition if any fan/pump is not on
					|| AFCHeartbeat.fan2Status == FAN_ERROR
					|| AFCHeartbeat.fan3Status == FAN_ERROR
					// && RSMHeartbeat.radiatorFanStatus == FAN_ON
					// && RSMHeartbeat.leftPumpStatus == PUMP_ON
					// && RSMHeartbeat.rightPumpStatus == PUMP_ON
					) 
				{
					nextState = ERROR;
				}
				
				// HV_EN -> GLV_ON
				else if(targetState == GLV_ON && nextState != ERROR) {
					nextState = GLV_ON;
				}
				
				// HV_EN -> RTD
				else if(targetState == RTD && nextState != ERROR) {
					if(CCHeartbeat.state == currentState
						&& FSMHeartbeat.state == currentState
						//&& RSMHeartbeat.state == currentState
						&& AFCHeartbeat.state == currentState
						// && DIMHeartbeat.state == currentState
						&& AFCHeartbeat.fan1Status == FAN_HIGH					// Make sure all fans/pumps turned on to high
						&& AFCHeartbeat.fan2Status == FAN_HIGH
						&& AFCHeartbeat.fan3Status == FAN_HIGH
						) 
						//&& adcVal(BPRES) > BRAKE_THRESH) 			// Brake pressed, removed for testing
					{
						nextState = RTD;
					}
				}
				break;
				
			case RTD:
				// State actions
				
				// RTD -> HV_EN
				if(targetState == HV_EN && nextState != ERROR) {
					if(CCHeartbeat.state == currentState
						&& FSMHeartbeat.state == currentState
						//&& RSMHeartbeat.state == currentState
						&& AFCHeartbeat.state == currentState
						// && DIMHeartbeat.state == currentState
						&& AFCHeartbeat.fan1Status == FAN_LOW					// Make sure all fans/pumps are set back to low
						&& AFCHeartbeat.fan2Status == FAN_LOW
						&& AFCHeartbeat.fan3Status == FAN_LOW
						) {
						nextState = HV_EN;
					}
				}
				break;
				
			case ERROR:
			case UNKNOWN:
				if(targetState == GLV_ON) {
					// DIM is trying to reset target state. Make sure it is safe to do so.
					if(!(AFCHeartbeatReceiveMeta.timeoutFlag			// Check that nothing is timing out
						// || RSMHeartbeatReceiveMeta.timeoutFlag
						|| FSMHeartbeatReceiveMeta.timeoutFlag
						|| DIMHeartbeatReceiveMeta.timeoutFlag
						|| CCHeartbeatReceiveMeta.timeoutFlag)
						&& CCHeartbeat.state == ERROR					// Check that all nodes are in error state
						&& FSMHeartbeat.state == ERROR
						// && RSMHeartbeat.state == ERROR
						&& AFCHeartbeat.state == ERROR
						// && DIMHeartbeat.state == ERROR
						&& AFCHeartbeat.fan1Status != FAN_ERROR			// Check that no fans have errors
						&& AFCHeartbeat.fan2Status != FAN_ERROR			
						&& AFCHeartbeat.fan3Status != FAN_ERROR
						// && RSMHeartbeat.radiatorFanStatus != FAN_ERROR
						) 
					{
						nextState = CLEAR_ERROR;
					}
				}
				break;
				
			case CLEAR_ERROR:
				if(targetState == GLV_ON) {
					// Make sure everything is in the clear error state before transioning back to GLV_ON
					if(CCHeartbeat.state == CLEAR_ERROR
						&& FSMHeartbeat.state == CLEAR_ERROR
						// && RSMHeartbeat.state == CLEAR_ERROR
						&& DIMHeartbeat.state == CLEAR_ERROR
						&& AFCHeartbeat.state == CLEAR_ERROR)
					{
						nextState = GLV_ON;
					}
				}
				break;
		}
		
		// Transition state
		currentState = nextState;
		
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod);  
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
		/* Central Controller                                                   */
		/************************************************************************/		
		
		// Central Controller Stale Check
		if(CCHeartbeatReceiveMeta.staleFlag && !CCHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment miss count if stale and not timed out
			CCHeartbeatReceiveMeta.missCount++;
		} else if (!CCHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			CCHeartbeatReceiveMeta.missCount = 0;
			CCHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Central Controller State Check
		if(CCHeartbeat.state != currentState && !CCHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			CCHeartbeatReceiveMeta.differentStateCount++;
		} else if(CCHeartbeat.state == currentState) {
			// Reset miss count if state matches
			CCHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Central Controller Hearbeat Timeout
		if(CCHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			// Set timeout if above timeout threshold
			CCHeartbeatReceiveMeta.timeoutFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			CCHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Central Controller State Timeout
		if(CCHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			CCHeartbeatReceiveMeta.wrongStateFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			CCHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
		
		/************************************************************************/
		/* Front Sensor Module                                                  */
		/************************************************************************/		
		
		// Front Sensor Module Stale Check
		if(FSMHeartbeatReceiveMeta.staleFlag && !FSMHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment if stale and not timed out
			FSMHeartbeatReceiveMeta.missCount++;
		} else if (!FSMHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			FSMHeartbeatReceiveMeta.missCount = 0;
			FSMHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Front Sensor Module State Check
		if(FSMHeartbeat.state != currentState && !FSMHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			FSMHeartbeatReceiveMeta.differentStateCount++;
		} else if(FSMHeartbeat.state == currentState) {
			// Reset miss count if state matches
			FSMHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Front Sensor Module Timeout
		if(FSMHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			FSMHeartbeatReceiveMeta.timeoutFlag = 1;
		} else {
			FSMHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Front Sensor Module State Timeout
		if(FSMHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			FSMHeartbeatReceiveMeta.wrongStateFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			FSMHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
		
		/************************************************************************/
		/* Rear Sensor Module                                                   */
		/************************************************************************/
		
		// Rear Sensor Module Stale Check
		if(RSMHeartbeatReceiveMeta.staleFlag && !RSMHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment if stale and not timed out
			RSMHeartbeatReceiveMeta.missCount++;
		} else if (!RSMHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			RSMHeartbeatReceiveMeta.missCount = 0;
			RSMHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Rear Sensor Module State Check
		if(RSMHeartbeat.state != currentState && !RSMHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			RSMHeartbeatReceiveMeta.differentStateCount++;
		} else if(RSMHeartbeat.state == currentState) {
			// Reset miss count if state matches
			RSMHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Rear Sensor Module Timeout
		if(RSMHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			RSMHeartbeatReceiveMeta.timeoutFlag = 1;
		} else {
			RSMHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Rear Sensor Module State Timeout
		if(RSMHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			RSMHeartbeatReceiveMeta.wrongStateFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			RSMHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
		
		/************************************************************************/
		/* Driver Interface Module                                              */
		/************************************************************************/
		
		// Driver Interface Module Stale Check
		if(DIMHeartbeatReceiveMeta.staleFlag && !DIMHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment if stale and not timed out
			DIMHeartbeatReceiveMeta.missCount++;
		} else if (!DIMHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			DIMHeartbeatReceiveMeta.missCount = 0;
			DIMHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Driver Interface Module State Check
		if(DIMHeartbeat.state != currentState && !DIMHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			DIMHeartbeatReceiveMeta.differentStateCount++;
		} else if(DIMHeartbeat.state == currentState) {
			// Reset miss count if state matches
			DIMHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Driver Interface Module Timeout
		if(DIMHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			DIMHeartbeatReceiveMeta.timeoutFlag = 1;
		} else {
			DIMHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Driver Interface Module State Timeout
		if(DIMHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			DIMHeartbeatReceiveMeta.wrongStateFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			DIMHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
		
		/************************************************************************/
		/* Accumulator Fan Controller                                           */
		/************************************************************************/
		
		// Accumulator Fan Controller Stale Check
		if(AFCHeartbeatReceiveMeta.staleFlag && !AFCHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment if stale and not timed out
			AFCHeartbeatReceiveMeta.missCount++;
		} else if (!AFCHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			AFCHeartbeatReceiveMeta.missCount = 0;
			AFCHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Accumulator Fan Controller State Check
		if(AFCHeartbeat.state != currentState && !AFCHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			AFCHeartbeatReceiveMeta.differentStateCount++;
		} else if(AFCHeartbeat.state == currentState) {
			// Reset miss count if state matches
			AFCHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Accumulator Fan Controller Timeout
		if(AFCHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			AFCHeartbeatReceiveMeta.timeoutFlag = 1;
		} else {
			AFCHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Accumulator Fan Controller State Timeout
		if(AFCHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			AFCHeartbeatReceiveMeta.wrongStateFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			AFCHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
		
		/************************************************************************/
		/* Telemetry Module                                                     */
		/************************************************************************/
		
		// Telemetry Module Stale Check
		if(TMHeartbeatReceiveMeta.staleFlag && !TMHeartbeatReceiveMeta.timeoutFlag) {
			// Only increment if stale and not timed out
			TMHeartbeatReceiveMeta.missCount++;
		} else if (!TMHeartbeatReceiveMeta.staleFlag) {
			// If not stale, reset miss count and set back to stale
			TMHeartbeatReceiveMeta.missCount = 0;
			TMHeartbeatReceiveMeta.staleFlag = 1;
		}
		// Telemetry Module State Check
		if(TMHeartbeat.state != currentState && !TMHeartbeatReceiveMeta.wrongStateFlag) {
			// Increment miss count if not the same and not timed out
			TMHeartbeatReceiveMeta.differentStateCount++;
		} else if(TMHeartbeat.state == currentState) {
			// Reset miss count if state matches
			TMHeartbeatReceiveMeta.differentStateCount = 0;
		}
		// Telemetry Module Timeout
		if(TMHeartbeatReceiveMeta.missCount > HEARTBEAT_TIMEOUT) {
			TMHeartbeatReceiveMeta.timeoutFlag = 1;
		} else {
			TMHeartbeatReceiveMeta.timeoutFlag = 0;
		}
		// Telemetry Module State Timeout
		if(TMHeartbeatReceiveMeta.differentStateCount > STATE_TIMEOUT) {
			// Set state timeout if above state timeout threshold
			TMHeartbeatReceiveMeta.wrongStateFlag = 1;
		} else {
			// Reset timeout flag if not timed out
			TMHeartbeatReceiveMeta.wrongStateFlag = 0;
		}
		
		// Delay until next period
		vTaskDelayUntil(&xLastWakeTime, xPeriod); 
	}
}

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