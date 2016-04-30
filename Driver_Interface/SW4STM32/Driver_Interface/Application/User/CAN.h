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
#include "cmr_constants.h"


typedef struct {
	uint32_t id;
	uint8_t length;
	uint8_t data[8];
} CanMessage;





//extern CanMessage * CanData;

/* 100Hz task freq = 10 ms period = 10,000us */
#define CAN_TASK_PERIOD (10*1000)

/* 1s state change timeout = 1,000,000us */
#define CAN_REQUEST_TIMEOUT_PERIOD (1000000/CAN_TASK_PERIOD)

#define CanData_idx(x) (x & 0b01111111)

extern osMessageQId stateButtonQueue;
extern CAN_HandleTypeDef hcan2;
extern CanTxMsgTypeDef TxMessage; /* Tx message struct */
extern CanRxMsgTypeDef RxMessage;



void UserInitCan2();
void vCanTask(void * pvParameters);
void CAN2SendMessage(uint8_t length, uint8_t *data);
NodeState nextState(NodeState DIstate);
NodeState prevState(NodeState DIstate);
NodeState getSafetyModuleStateCAN();



#endif /* APPLICATION_USER_CAN_H_ */
