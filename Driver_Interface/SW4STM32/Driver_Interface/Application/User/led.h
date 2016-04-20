/*
 * led.h
 *
 *  Created on: Apr 9, 2016
 *      Author: Tom
 */

#ifndef APPLICATION_USER_LED_H_
#define APPLICATION_USER_LED_H_
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

typedef uint32_t portTickType;
void vLedUpdateTask(void * pvParameters);

#endif /* APPLICATION_USER_LED_H_ */
