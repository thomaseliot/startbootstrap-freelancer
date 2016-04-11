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
osMessageQId buttonQueue;


typedef enum {
	NEXT_BTN,
	PREV_BTN,
	SELECT_BTN,
	RETURN_BTN,
	UP_BTN,
	DOWN_BTN
} button;

typedef enum {
	PRESSED = 0,
	UN_PRESSED = 1
} buttonState;

typedef struct {
	button name;
	buttonState state;
} ButtonObject;

void vPollButtonsTask(void * pvParameters);
buttonState getButtonState(GPIO_PinState pinState);
void updateButton(GPIO_TypeDef * GPIOx, uint16_t GPIO_pin, ButtonObject buttonObject);

#endif /* APPLICATION_USER_TASKS_H_ */
