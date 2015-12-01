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
#include "can_lib.h"

//IO pin configuration macros, to choose ports and directions
#define IO_PORT_B		0
#define IO_PORT_C		1
#define IO_PORT_D		2
#define IO_DIR_INPUT	0
#define IO_DIR_OUTPUT	1

#ifndef TASKS_H_
#define TASKS_H_

void vLEDFlashTask(void *pvParameters);
void config_io_pin(uint8_t port, uint8_t port_ch, uint8_t dir);
void setup_can_send(st_cmd_t *can_send_msg, uint8_t *can_send_buffer, uint8_t buffer_len, uint16_t msg_id);
void can_tx(uint16_t id, uint8_t *buffer, uint8_t buf_size);
void vHeartbeatTask(void *pvParameters);

#endif /* TASKS_H_ */