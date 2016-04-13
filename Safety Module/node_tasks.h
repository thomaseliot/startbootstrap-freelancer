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
#include "frtos_can.h"
#include "can_config.h"

// Task priorities
#define MCU_STATUS_TASK_PRIORITY		1
#define ADC_SAMPLE_TASK_PRIORITY		4
#define HEARTBEAT_TASK_PRIORITY			2

// Task rates, in Hz
#define MCU_STATUS_TASK_RATE			4
#define ADC_SAMPLE_TASK_RATE			100
#define HEARTBEAT_TASK_RATE				10


// Task functions
void vMCUStatusTask(void *pvParameters);
void vADCSampleTask(void *pvParameters);
void vHeartbeatTask(void *pvParameters);
void vCANSendTask(void *pvParameters);
void vCANReceiveTask(void *pvParameters);

#endif /* TASKS_H_ */