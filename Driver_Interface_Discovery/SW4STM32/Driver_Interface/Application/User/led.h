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

/* 100Hz task freq = 10 ms period = 10,000us */
#define period (10*10000)

extern I2C_HandleTypeDef hi2c3;

typedef uint32_t color_t;
typedef uint8_t led_id_t;

typedef uint32_t portTickType;

#define I2C_TIMEOUT osKernelSysTickMicroSec(10)

#define get_blue(c) (c&0xFF)
#define get_green(c) ((c>>8)&0xFF)
#define get_red(c) ((c>>16)&0xFF)
#define get_alpha(c) ((c>>24)&0xFF)
#define make_color(c,a) ((a<<24)|c)

#define COLOR_RED make_color(0xFF0000, 0xFF)
#define COLOR_BLUE make_color(0x00FF00, 0xFF)
#define COLOR_GREEN make_color(0x00F00, 0xFF)

#define DRV1 (96<<1)
#define DRV2 (97<<1)
#define DRV3 (98<<1)
#define DRV4 (99<<1)
#define DRV5 (100<<1)
#define DRV6 (101<<1)


void vLedUpdateTask(void * pvParameters);
void set_led(led_id_t * led_id, color_t led_color);
void ledInitAll();
void ledInit(int8_t led_addr);

#endif /* APPLICATION_USER_LED_H_ */
