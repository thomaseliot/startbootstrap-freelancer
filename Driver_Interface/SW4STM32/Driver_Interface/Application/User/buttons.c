/*
 * tasks.c
 *
 *  Created on: Apr 9, 2016
 *      Author: Tom
 */
#include "buttons.h"

void vPollButtonsTask(void * pvParameters) {

	portTickType xLastWakeTime;
	xLastWakeTime = osKernelSysTick();



	ButtonObject NEXT_BTN_object = {.name = NEXT_BTN, .state = UN_PRESSED};
	ButtonObject PREV_BTN_object = {.name = PREV_BTN, .state = UN_PRESSED};
	ButtonObject SELECT_BTN_object = {.name = SELECT_BTN, .state = UN_PRESSED};
	ButtonObject RETURN_BTN_object = {.name = RETURN_BTN, .state = UN_PRESSED};
	ButtonObject UP_BTN_object = {.name = UP_BTN, .state = UN_PRESSED};
	ButtonObject DOWN_BTN_object = {.name = DOWN_BTN, .state = UN_PRESSED};

	for( ;; )
	{
		/* Poll GPIO */
		updateButton(NEXT_BTN_GPIO_Port, NEXT_BTN_Pin, &NEXT_BTN_object);
		updateButton(PREV_BTN_GPIO_Port, PREV_BTN_Pin, &PREV_BTN_object);
		updateButton(SELECT_BTN_GPIO_Port, SELECT_BTN_Pin, &SELECT_BTN_object);
		updateButton(RETURN_BTN_GPIO_Port, RETURN_BTN_Pin, &RETURN_BTN_object);
		updateButton(UP_BTN_GPIO_Port, UP_BTN_Pin, &UP_BTN_object);
		updateButton(DOWN_BTN_GPIO_Port, DOWN_BTN_Pin, &DOWN_BTN_object);


		/* 10ms period = 100Hz */
		osDelayUntil( &xLastWakeTime, ( osKernelSysTickMicroSec(10*1000) ) );
	}
}

void updateButton(GPIO_TypeDef * GPIOx, uint16_t GPIO_pin, ButtonObject * buttonObject){
	if (buttonObject->state == getButtonState(HAL_GPIO_ReadPin(GPIOx, GPIO_pin))) {
			;;
		}
	else {
		//Button changed
		buttonObject->state = ! buttonObject->state;
		//Send button change message
		uint32_t buttonMsg = * (uint32_t *) buttonObject;
		osStatus messageStatus = osMessagePut(buttonQueue, buttonMsg, 0);

	}

	return;

}


//Active low buttons
buttonState getButtonState(GPIO_PinState pinState) {
	if (pinState == GPIO_PIN_SET)
		return UN_PRESSED;
	else
		return PRESSED;
}
