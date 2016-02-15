/* 
 * tasks.h
 * File containing tasks to be run by the RTOS
 * 
 * Created: 11/23/2015 6:19:57 PM
 * Author: Shepard Emerson (semerson)
 * 
 * Copyright (c) 2015, Carnegie Mellon Racing
 * 
 */ 

#include "FreeRTOS.h"
#include "task.h"
#include "can.h"

//IO pin configuration macros, to choose ports and directions
#define IO_PORT_B		0
#define IO_PORT_C		1
#define IO_PORT_D		2
#define IO_DIR_INPUT	0
#define IO_DIR_OUTPUT	1

#ifndef TASKS_H_
#define TASKS_H_

void vMCUStatusTask(void *pvParameters);
void vHeartbeatTask(void *pvParameters);

#endif /* TASKS_H_ */