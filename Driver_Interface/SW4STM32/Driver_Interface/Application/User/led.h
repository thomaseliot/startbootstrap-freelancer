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
#include "can.h"

/* 60Hz task freq = 16.667 ms period */
#define LED_TASK_PERIOD (16667)

extern I2C_HandleTypeDef hi2c3;
extern CanMessage * CanData;
extern NodeState DIstate;
extern NodeState requestedState;


typedef uint32_t color_t;
typedef uint8_t led_id_t;
//typedef enum {
//	SOLID,
//	GRAD,
//} barcolor_scheme_t;
//typedef struct {
//	barcolor_scheme_t scheme;
//	color_t solid_color;
//	color_t grad_start;
//	color_t grad_end;
//} barstyle_t;

typedef struct {
	color_t start;
	color_t end;
} color_grad_t;

typedef uint32_t portTickType;

extern LTDC_HandleTypeDef hltdc;


#define I2C_TIMEOUT 1

#define LED_DRV_OE_L_Pin GPIO_PIN_4
#define LED_DRV_OE_L_GPIO_Port GPIOE


#define get_blue(c) (c&0xFF)
#define get_green(c) ((c>>8)&0xFF)
#define get_red(c) ((c>>16)&0xFF)
#define get_alpha(c) ((c>>24)&0xFF)
#define make_color(c,a) ((a<<24)|c)

#define set_alpha(c,a) ((c&0x00FFFFFF)| (a<<24))

#define COLOR_RED make_color(0xFF0000, 0xFF)
#define COLOR_BLUE make_color(0x0000FF, 0xFF)
#define COLOR_GREEN make_color(0x00FF00, 0xFF)
#define COLOR_YELLOW make_color(0xFFFF00, 0xFF)
#define COLOR_WHITE make_color(0xFFFFFF, 0x20)



#define LED_DRV_PWM_REG_OFFSET 2
#define DRV1 (96<<1)
#define DRV2 (97<<1)
#define DRV3 (98<<1)
#define DRV4 (99<<1)
#define DRV5 (100<<1)
#define DRV6 (101<<1)

#define LED_TOP_LEN 16
#define LED_LEFT_LEN 8
#define LED_RIGHT_LEN 8

#define LED_BRIGHTNESS 75


void vLedUpdateTask(void * pvParameters);
HAL_StatusTypeDef set_led(led_id_t * led_id, color_t led_color, uint8_t scale);
void ledInitAll();
HAL_StatusTypeDef ledInit(int8_t led_addr);
int32_t map_val(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max);
int32_t getSpeedCAN();
int32_t getSOCCAN();
int8_t getLedBrightnessCan();
int32_t getMaxBattTempCAN();
void I2C_update_leds();
void LedOn(GPIO_TypeDef * Port, uint16_t Pin);
void LedOff(GPIO_TypeDef * Port, uint16_t Pin);
int8_t getErrorMatrixCAN();
void UpdateErrorLeds();
void UpdateLedBars();
void LedSetBarPercent(int32_t percentage, color_t * led_bar, int8_t led_bar_len);
void LedSetBarColorSolid(color_t bar_color, color_t * led_bar, int8_t led_bar_len);
void LedSetBarColorArray(color_t * bar_color, color_t * led_bar, int8_t led_bar_len);

#endif /* APPLICATION_USER_LED_H_ */
