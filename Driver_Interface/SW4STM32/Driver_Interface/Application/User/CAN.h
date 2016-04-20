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


//extern uint8_t CAN1dataReceivedFlag;






typedef struct {
	uint32_t id;
	uint8_t length;
	uint8_t data[8];
} CanMessage;

void vCanStart(void * pvParameters);



//osPoolDef (CanData_Pool, 128, CanMessage);
//osPoolId   CanData_Pool_Id;

void InitializeCANBUS1();
void CAN1SendMessage(uint8_t length, uint8_t *data);

extern CAN_HandleTypeDef hcan1;


#endif /* APPLICATION_USER_CAN_H_ */
