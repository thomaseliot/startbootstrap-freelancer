/* 
 * node_tasks.h
 * 
 * File containing tasks to be run by the RTOS
 * 
 * Copyright (c) 2016, Carnegie Mellon Racing
 */ 

#include "FreeRTOS.h"
#include "task.h"

#ifndef NODE_TASKS_H_
#define NODE_TASKS_H_

/* Drivers */
#include "adc.h"
#include "pwm.h"
#include "can.h"
#include "can_config.h"
#include "frtos_can.h"

//CAN definitions
#include "can_structs.h"
#include "can_ids.h"


// Task priorities
#define MCU_STATUS_TASK_PRIORITY		2
#define ADC_SAMPLE_TASK_PRIORITY		4
#define COOLING_UPDATE_TASK_PRIORITY	4
#define COOLING_SET_TASK_PRIORITY		3
#define HEARTBEAT_TASK_PRIORITY			4


// Task rates, in Hz	
#define HEARTBEAT_TASK_RATE				100
#define MCU_STATUS_TASK_RATE			4
#define ADC_SAMPLE_TASK_RATE			100
#define COOLING_SET_TASK_RATE			20
#define COOLING_UPDATE_TASK_RATE		100

//Length of ramp in seconds; max is 13s.
#define RAMP_LENGTH						5

//Fan States
#define FAN_OFF							0
#define FAN_RAMP						1
#define FAN_ON							2
#define FAN_ERROR						3


// Task functions
void vMCUStatusTask(void *pvParameters);
void vADCSampleTask(void *pvParameters);
void vHeartbeatTask(void *pvParameters);
void vCANSendTask(void *pvParameters);
void vCANReceiveTask(void *pvParameters);
void vFanTestTask(void *pvParameters);
void vCoolingSetTask(void *pvParameters);
void vCoolingUpdateTask(void *pvParameters);

//Fan globals
uint8_t fan_duty;
uint8_t cooling_state;
uint8_t leftPumpStatus;
uint8_t rightPumpStatus;


#endif /* TASKS_H_ */