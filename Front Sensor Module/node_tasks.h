/* 
 * node_tasks.h
 * 
 * File containing tasks to be run by the RTOS
 * 
 * Copyright (c) 2016, Carnegie Mellon Racing
 */  

#include "FreeRTOS.h"
#include "task.h"
#include "can.h"
#include "can_config.h"
#include "frtos_can.h"

#ifndef NODE_TASKS_H_
#define NODE_TASKS_H_

// Task priorities
#define MCU_STATUS_TASK_PRIORITY		1
#define ADC_SAMPLE_TASK_PRIORITY		4
#define WATCHDOG_TASK_PRIORITY			5
#define THERMISTOR_READ_TASK_PRIORITY	3
#define THERMISTOR_WRITE_TASK_PRIORITY	3

// Task rates, in Hz
#define MCU_STATUS_TASK_RATE			4
#define ADC_SAMPLE_TASK_RATE			100
#define WATCHDOG_TASK_RATE				4
#define THERMISTOR_READ_TASK_RATE		100
#define THERMISTOR_WRITE_TASK_RATE		10

// Global variables
extern MOB_STATUS statuses[NO_MOBS];

void vMCUStatusTask(void *pvParameters);
void vHeartbeatTask(void *pvParameters);
void vCANSendTask(void *pvParameters);
void vCANReceiveTask(void *pvParameters);

#endif /* TASKS_H_ */