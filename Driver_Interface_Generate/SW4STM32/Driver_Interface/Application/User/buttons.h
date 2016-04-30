/*
 * tasks.h
 *
 *  Created on: Apr 9, 2016
 *      Author: Tom
 */

#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#ifndef APPLICATION_USER_TASKS_H_
#define APPLICATION_USER_TASKS_H_

typedef uint32_t portTickType;


typedef enum {
	NEXT_BTN,
	PREV_BTN,
	SELECT_BTN,
	RETURN_BTN,
	UP_BTN,
	DOWN_BTN
} button;

typedef enum {
	BTN_PRESSED = 0,
	BTN_UN_PRESSED = 1
} buttonState;

typedef struct {
	button name;
	buttonState state;
} ButtonObject;

extern osMessageQId stateButtonQueue;
extern osMessageQId interfaceButtonQueue;
void vPollButtonsTask(void * pvParameters);
buttonState getButtonState(GPIO_PinState pinState);
void updateButton(GPIO_TypeDef * GPIOx, uint16_t GPIO_pin, ButtonObject * buttonObject, osMessageQId buttonQueue);

#endif /* APPLICATION_USER_TASKS_H_ */
