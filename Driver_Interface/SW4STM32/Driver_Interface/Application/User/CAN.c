#include "can.h"
//#include "cmsis_os.h"
//#include "stm32f4xx_hal_gpio.h"
#include "stm32f4xx_hal_can.h"
#include "can_structs.h"
#include "can_ids.h"

//http://eraycanli.com/2015/09/28/stm32f4-discovery-can-bus-communication/
uint8_t CAN1dataReceivedFlag;


CanTxMsgTypeDef TxMessage; /* Tx message struct */
CanRxMsgTypeDef RxMessage; /* Rx message struct */

/*	Max 2^7 = 128 unique messages */
osPoolDef (CanData_Pool, 256, CanMessage);
osPoolId (CanData_Pool_id);
CanMessage * CanData;



void vCanTask(void * pvParameters) {
	portTickType xLastWakeTime;
	NodeState DIstate;
	NodeState requestedState;
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
				recvdButton = *(ButtonObject *) recvdMessage.value.p;
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
		if(timeoutTick >= timeoutPeriod){
			requestedState = DIstate;
		}


		//Change DASH state if safety module has initiated state change on CAN
		DIstate = getSafetyModuleStateCAN();


		//TODO change to heartbeat
		DIM_heartbeat.state = DIstate;
		DIM_heartbeat.requestedState = requestedState;
		DIM_heartbeat.vbatt = 0; //TODO: placeholder
		CAN1SendMessage(sizeof(DIMHeartbeat_t), &DIM_heartbeat);

		timeoutTick++;
		osDelayUntil( &xLastWakeTime, ( osKernelSysTickMicroSec(period) ) );
	}
}

//typedef enum {STATE_GLV_ON, STATE_HV_EN, STATE_RTD, STATE_ERROR, STATE_UNKNOWN} NodeState;

NodeState nextState(NodeState DIstate) {
	switch(DIstate) {
		case STATE_UNKNOWN:
			return STATE_UNKNOWN;
		case STATE_ERROR:
			return STATE_GLV_ON;
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
			return STATE_RTD;
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

void InitializeCANBUS1()
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_FilterConfTypeDef CAN_FilterStruct;


	__HAL_RCC_CAN1_CLK_ENABLE(); /* Enable CANBUS-1 clock */

	__GPIOA_CLK_ENABLE(); /* Enable GPIOD clock */


	//TODO: Disco board CAN: PA11 = RX, PA12 = TX
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; /* Alternate Function Push-Pull mode */
	GPIO_InitStruct.Pull = GPIO_NOPULL; /* No resistor */
	GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF9_CAN1; /* Alternate pin function: AF9 / CAN1 */
	GPIO_InitStruct.Pin = GPIO_PIN_12 | GPIO_PIN_11; /* PA12(CAN1_TX) & PA11(CAN1_RX) */
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct); /* Initialize registers */
	//TODO: unbreak the rest of GPIOA

//
//	hcan2.Instance = CAN1;
//
//	hcan2.Init.Prescaler = 19; /* Time quanta prescaler (tq = (Prescaler+1)/42MHz) */
//	hcan2.Init.Mode = CAN_MODE_NORMAL; /* Normal communication mode */
//	hcan2.Init.SJW = CAN_SJW_4TQ; /* 4 time-quanta re-sync jump width */
//	hcan2.Init.BS1 = CAN_BS1_9TQ; /* 9 tq before sampling point */
//	hcan2.Init.BS2 = CAN_BS2_8TQ; /* 8 tq after sampling point (before end) */
//	hcan2.Init.TTCM = DISABLE; /* Time-triggered communication */
//	hcan2.Init.ABOM = DISABLE; /* Automatic bus-off management */
//	hcan2.Init.AWUM = DISABLE; /* Automatic wake-up mode */
//	hcan2.Init.NART = DISABLE; /* Non-automatic retransmission mode */
//	hcan2.Init.RFLM = DISABLE; /* Receive FIFO locked mode */
//	hcan2.Init.TXFP = DISABLE; /* Transmit FIFO priority */
//	HAL_CAN_Init(&hcan2); /* Init CAN1 registers */


	  hcan1.pTxMsg = &TxMessage; /* Pointer to CAN Tx message */
	  hcan1.pRxMsg = &RxMessage; /* Pointer to CAN Rx message */

	  hcan1.Instance = CAN1;
	  hcan1.Init.Prescaler = 2;
	  hcan1.Init.Mode = CAN_MODE_NORMAL;
	  hcan1.Init.SJW = CAN_SJW_1TQ;
	  hcan1.Init.BS1 = CAN_BS1_13TQ;
	  hcan1.Init.BS2 = CAN_BS2_2TQ;
	  hcan1.Init.TTCM = DISABLE;
	  hcan1.Init.ABOM = DISABLE;
	  hcan1.Init.AWUM = DISABLE;
	  hcan1.Init.NART = DISABLE;
	  hcan1.Init.RFLM = DISABLE;
	  hcan1.Init.TXFP = DISABLE;
	  HAL_CAN_Init(&hcan1);

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
	HAL_CAN_ConfigFilter(&hcan1, &CAN_FilterStruct); /* Initialize filter */

//
//	HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 1); /* Set CAN1 Rx interrupt priority to 1-1 */
//	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn); /* Enable CAN1 Rx Interrupt */



    //TODO: disco board CAN fix
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 1, 1);
	HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
//
//	__HAL_CAN_ENABLE_IT(&hcan1, CAN_IT_FMP0); /* Enable 'message pending in FIFO0' interrupt */
	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);

	//TODO: set node ID
	hcan1.pTxMsg->StdId = 0b01000000110; /* Standard ID of this device */
	// 100Hz Hearbeat DI-module
	// 010   0000     0110
//	hcan1.pTxMsg->ExtId = 0x01; /* Extended ID */
	hcan1.pTxMsg->RTR = CAN_RTR_DATA; /* Remote transmission request:data */
	hcan1.pTxMsg->IDE = CAN_ID_STD; /* Identifier type: standard */

	/* Create memory block to store arriving CAN data */
	CanData_Pool_id = osPoolCreate (osPool (CanData_Pool));
		if (CanData_Pool_id != NULL)  {
		    // allocate a memory block
			CanData = (CanMessage *)osPoolCAlloc (CanData_Pool_id);

				if (CanData != NULL) {
				  // memory block was allocated
				}
		}


	return;
}


void CAN1SendMessage(uint8_t length, uint8_t *data)
{
	hcan1.pTxMsg->DLC = length; /* Specify the data length */

	uint8_t d;

	for(d = 0; d < length; ++d)
	{
		hcan1.pTxMsg->Data[d] = *(data + d); /* Write every byte to Data[] variable */
	}

	HAL_CAN_Transmit_IT(&hcan1);
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

	HAL_CAN_Receive_IT(&hcan1, CAN_FIFO0);
	return;

}

void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN2_RX0_IRQn 0 */

  /* USER CODE END CAN2_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN2_RX0_IRQn 1 */

  /* USER CODE END CAN2_RX0_IRQn 1 */
}


