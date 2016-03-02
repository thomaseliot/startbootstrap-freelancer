/* 
 * node_tasks.h
 * 
 * File containing tasks to be run by the RTOS
 * 
 * Copyright (c) 2016, Carnegie Mellon Racing
 */ 

#include "FreeRTOS.h"
#include "task.h"

#ifndef TASKS_H_
#define TASKS_H_

/* Drivers */
#include "adc.h"
#include "spi.h"

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

// Globals
static uint16_t maxThermistorValue;

// Task functions
void vMCUStatusTask(void *pvParameters);
void vADCSampleTask(void *pvParameters);
void vWatchdogTask(void *pvParameters);
void vThermistorReadTask(void *pvParameters);
void vThermistorWriteTask(void *pvParameters);

#endif /* TASKS_H_ */