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

//BGR
//typedef struct {
//	int8_t b_driver;
//	int8_t b_pin;
//	int8_t g_driver;
//	int8_t g_pin;
//	int8_t r_driver;
//	int8_t r_pin;
//} led_t;
//
//#define BAR_LED_1 4,0,4,1,4,2
//#define BAR_LED_2 4,3,4,4,4,5
//#define BAR_LED_3 4,6,4,7,4,8
//#define BAR_LED_4

//1 byte each: alpha R G B
typedef uint32_t color_t;

typedef uint32_t portTickType;

//#define I2C_TIMEOUT

#define get_blue(c) (c&0xFF)
#define get_green(c) ((c>>8)&0xFF)
#define get_red(c) ((c>>16)&0xFF)
#define get_alpha(c) ((c>>24)&0xFF)

#define LED_DRV_1_ADR_W (96<<1)
#define LED_DRV_2_ADR_W (97<<1)
#define LED_DRV_3_ADR_W (98<<1)
#define LED_DRV_4_ADR_W (99<<1)
#define LED_DRV_5_ADR_W (100<<1)
#define LED_DRV_6_ADR_W (101<<1)


void vLedUpdateTask(void * pvParameters);

#endif /* APPLICATION_USER_LED_H_ */
