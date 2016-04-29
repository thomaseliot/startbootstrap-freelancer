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
#include "spi.h"
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
#define TEMP_SAMPLE_TASK_PRIORITY		4
#define FAN_UPDATE_TASK_PRIORITY		4
#define FAN_SET_TASK_PRIORITY		    3
#define HEARTBEAT_TASK_PRIORITY			4


// Task rates, in Hz
#define HEARTBEAT_TASK_RATE				100
#define MCU_STATUS_TASK_RATE			4
#define ADC_SAMPLE_TASK_RATE			100
#define TEMP_SAMPLE_TASK_RATE			100
#define FAN_SET_TASK_RATE				20
#define FAN_UPDATE_TASK_RATE			100

//Length of ramp in seconds; max is 13s.
#define RAMP_LENGTH						5

#define MAX_DUTY						127

//Fan States
#define FAN_OFF							0
#define FAN_RAMP_UP						1
#define FAN_ON							2
#define FAN_ERROR						3
#define FAN_RAMP_DOWN					4


// Task functions
void vMCUStatusTask(void *pvParameters);
void vADCSampleTask(void *pvParameters);
void vHeartbeatTask(void *pvParameters);
void vCANSendTask(void *pvParameters);
void vCANReceiveTask(void *pvParameters);
void vFanTestTask(void *pvParameters);
void vFanSetTask(void *pvParameters);
void vFanUpdateTask(void *pvParameters);

//Fan globals
uint8_t fan_duty;
uint8_t fan_state;
//Fixed point signed number, 5 bits of fraction
int16_t sysTemp;

#endif /* TASKS_H_ */