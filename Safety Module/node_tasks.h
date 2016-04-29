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
#include "can_structs.h"
#include "cmr_64c1_lib.h"

// Task priorities
#define MCU_STATUS_TASK_PRIORITY		1
#define ADC_SAMPLE_TASK_PRIORITY		2
#define HEARTBEAT_TASK_PRIORITY			3
#define STATE_SEQUENCE_TASK_PRIORITY	4
#define TIMEOUT_MONITOR_TASK_PRIORITY	4

// Task rates, in Hz
#define MCU_STATUS_TASK_RATE			4
#define ADC_SAMPLE_TASK_RATE			100
#define HEARTBEAT_TASK_RATE				100
#define STATE_SEQUENCE_TASK_RATE		100
#define TIMEOUT_MONITOR_TASK_RATE		100

// Task functions
void initTasks(void);
void vMCUStatusTask(void *pvParameters);
void vADCSampleTask(void *pvParameters);
void vHeartbeatTask(void *pvParameters);
void vCANReceiveTask(void *pvParameters);
void vCANStateSequenceTask(void *pvParameters);
void vCANTimeoutMonitorTask(void *pvParameters);

#endif /* TASKS_H_ */