/*
 * can.h
 *
 *  Created on: Apr 9, 2016
 *      Author: Tom
 */

#ifndef APPLICATION_USER_CAN_H_
#define APPLICATION_USER_CAN_H_
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "buttons.h"



typedef struct {
	uint32_t id;
	uint8_t length;
	uint8_t data[8];
} CanMessage;

typedef enum {STATE_GLV_ON, STATE_HV_EN, STATE_RTD, STATE_ERROR, STATE_UNKNOWN} NodeState;




extern CanMessage * CanData;

/* 100Hz task freq = 10 ms period = 10,000us */
#define PERIOD (10*1000)

/* 1s state change timeout = 1,000,000us */
#define TIMEOUT_PERIOD (1000000/PERIOD)

#define CanData_idx(x) (x & 0b01111111)

extern osMessageQId stateButtonQueue;
extern CAN_HandleTypeDef hcan1;



void InitializeCANBUS1();
void vCanTask(void * pvParameters);
void CAN1SendMessage(uint8_t length, uint8_t *data);
NodeState nextState(NodeState DIstate);
NodeState prevState(NodeState DIstate);
NodeState getSafetyModuleStateCAN();



#endif /* APPLICATION_USER_CAN_H_ */
