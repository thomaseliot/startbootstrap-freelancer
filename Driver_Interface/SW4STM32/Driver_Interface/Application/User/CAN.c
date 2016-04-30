#include "can.h"
//#include "cmsis_os.h"
//#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"
#include "can_ids.h"
#include "can_structs.h"


//http://eraycanli.com/2015/09/28/stm32f4-discovery-can-bus-communication/
uint8_t CAN1dataReceivedFlag;


CanTxMsgTypeDef TxMessage; /* Tx message struct */
CanRxMsgTypeDef RxMessage; /* Rx message struct */

/*	Max 2^7 = 128 unique messages */
osPoolDef (CanData_Pool, 256, CanMessage);
osPoolId (CanData_Pool_id);
CanMessage * CanData;
NodeState DIstate;
NodeState requestedState;





void vCanTask(void * pvParameters) {
	portTickType xLastWakeTime;
	osEvent recvdMessage;
	ButtonObject recvdButton;
	DIMHeartbeat_t DIM_heartbeat;

	xLastWakeTime = osKernelSysTick();
	int timeoutTick = 0;
	DIstate = STATE_UNKNOWN;
	requestedState = STATE_UNKNOWN;


	for( ;; ) {


		//Receive button info from queue & request state change
		do {
			recvdMessage = osMessageGet(stateButtonQueue, 0);
			if(recvdMessage.status == osEventMessage) {
				recvdButton = *((ButtonObject *) (& recvdMessage.value.v));
				if(requestedState == DIstate) { //state change should not already be in progress
					if(recvdButton.name == NEXT_BTN && recvdButton.state == BTN_UN_PRESSED) {
						requestedState = nextState(DIstate);
					}
					else if(recvdButton.name == PREV_BTN && recvdButton.state == BTN_UN_PRESSED) {
						requestedState = prevState(DIstate);
					}
					timeoutTick = 0;
				}
			}
		}while(recvdMessage.status == osEventMessage);

		//On timeout, stop requesting state change
		if(timeoutTick >= CAN_REQUEST_TIMEOUT_PERIOD){
			requestedState = DIstate;
		}


		//Change DASH state if safety module has initiated state change on CAN
		DIstate = getSafetyModuleStateCAN();



		//TODO change to heartbeat
		DIM_heartbeat.state = DIstate;
		DIM_heartbeat.requestedState = requestedState;
		DIM_heartbeat.vbatt = 0; //TODO: placeholder
		CAN2SendMessage(sizeof(DIMHeartbeat_t), &DIM_heartbeat);

		timeoutTick++;
		HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);
		osDelayUntil( &xLastWakeTime, ( osKernelSysTickMicroSec(CAN_TASK_PERIOD) ) );
	}
}

//typedef enum {STATE_GLV_ON, STATE_HV_EN, STATE_RTD, STATE_ERROR, STATE_UNKNOWN} NodeState;

NodeState nextState(NodeState DIstate) {
	switch(DIstate) {
		case STATE_UNKNOWN:
			return STATE_UNKNOWN;
		case STATE_ERROR:
			return STATE_CLEAR_ERROR;
		case STATE_GLV_ON:
			return STATE_HV_EN;
		case STATE_HV_EN:
			return STATE_RTD;
		case STATE_RTD:
			return STATE_RTD;
		default:
			return STATE_ERROR;
	}
}

NodeState prevState(NodeState DIstate) {
	switch(DIstate) {
		case STATE_UNKNOWN:
			return STATE_UNKNOWN;
		case STATE_ERROR:
			return STATE_ERROR;
		case STATE_GLV_ON:
			return STATE_GLV_ON;
		case STATE_HV_EN:
			return STATE_GLV_ON;
		case STATE_RTD:
			return STATE_HV_EN;
		default:
			return STATE_ERROR;
	}
}

NodeState getSafetyModuleStateCAN() {
	CanMessage SMmsg;
	SMmsg = CanData[CanData_idx(SM_HEARTBEAT_ID)];
	if(SMmsg.length) {
		SMHeartbeat_t SMhbeat = * (SMHeartbeat_t *) SMmsg.data;
		return SMhbeat.state;
	}
	else {
		return STATE_ERROR;
	}
}

void UserInitCan2()
{

	CAN_FilterConfTypeDef CAN_FilterStruct;




	CAN_FilterStruct.FilterIdHigh = 0x0000; /* Upper 16bit filter ID */
	CAN_FilterStruct.FilterIdLow = 0x0000; /* Filter lower 16bit ID */
	CAN_FilterStruct.FilterMaskIdHigh = 0x0000; /* Upper 16bit filter mask */
	CAN_FilterStruct.FilterMaskIdLow = 0x0000; /* Lower 16bit filter mask */
	CAN_FilterStruct.FilterFIFOAssignment = CAN_FILTER_FIFO0; /* Which FIFO will be assigned to filter */
	CAN_FilterStruct.FilterNumber = 0;
	CAN_FilterStruct.FilterMode = CAN_FILTERMODE_IDMASK; /* Identifier mask mode */
	CAN_FilterStruct.FilterScale = CAN_FILTERSCALE_32BIT; /* 32bit ID filter */
	CAN_FilterStruct.FilterActivation = ENABLE; /* Enable this filter */
	CAN_FilterStruct.BankNumber = 14; /* Start slave bank filter (?) */
	HAL_CAN_ConfigFilter(&hcan2, &CAN_FilterStruct); /* Initialize filter */

//
//	HAL_NVIC_SetPriority(CAN2_RX0_IRQn, 1, 1); /* Set CAN1 Rx interrupt priority to 1-1 */
//	HAL_NVIC_EnableIRQ(CAN2_RX0_IRQn); /* Enable CAN1 Rx Interrupt */



    //TODO: disco board CAN fix
//
//	__HAL_CAN_ENABLE_IT(&hcan2, CAN_IT_FMP0); /* Enable 'message pending in FIFO0' interrupt */

	hcan2.pTxMsg->StdId = DIM_HEARTBEAT_ID; /* Standard ID of this device */
	hcan2.pTxMsg->RTR = CAN_RTR_DATA; /* Remote transmission request:data */
	hcan2.pTxMsg->IDE = CAN_ID_STD; /* Identifier type: standard */

	/* Create memory block to store arriving CAN data */
	CanData_Pool_id = osPoolCreate (osPool (CanData_Pool));
		if (CanData_Pool_id != NULL)  {
		    // allocate a memory block
			CanData = (CanMessage *)osPoolCAlloc (CanData_Pool_id);

				if (CanData != NULL) {
				  // memory block was allocated
				}
		}

	HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);



	return;
}


void CAN2SendMessage(uint8_t length, uint8_t *data)
{
	hcan2.pTxMsg->DLC = length; /* Specify the data length */

	uint8_t d;

	for(d = 0; d < length; ++d)
	{
		hcan2.pTxMsg->Data[d] = *(data + d); /* Write every byte to Data[] variable */
	}

	HAL_CAN_Transmit_IT(&hcan2	);
	//TODO: error handling
	//HAL_CAN_Transmit(&hcan1, 3000); /* Transmit the packet */

	return;
}


void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {

	CanRxMsgTypeDef Message;
	Message = * (hcan->pRxMsg);
	uint8_t idx = CanData_idx(Message.StdId);

	CanData[idx].id = Message.StdId;
	CanData[idx].length = Message.DLC;
	for(uint8_t i =0; i < Message.DLC; i++){
		CanData[idx].data[i] = Message.Data[i];
	}

	/* Seems like if this call fails, the interrupt never happens again and CAN
	   stops being received, so we restart it every 0.01s in the hearbeat task.*/
	HAL_CAN_Receive_IT(&hcan2, CAN_FIFO0);
	return;

}
//in stm32f4xx_it.c
//void CAN2_RX0_IRQHandler(void)
//{
//  /* USER CODE BEGIN CAN2_RX0_IRQn 0 */
//
//  /* USER CODE END CAN2_RX0_IRQn 0 */
//  HAL_CAN_IRQHandler(&hcan2);
//  /* USER CODE BEGIN CAN2_RX0_IRQn 1 */
//
//  /* USER CODE END CAN2_RX0_IRQn 1 */
//}


