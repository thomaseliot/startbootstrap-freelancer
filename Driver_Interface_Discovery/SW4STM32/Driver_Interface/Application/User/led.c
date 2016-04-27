#include "led.h"
#include "mxconstants.h"
#include "stm32f4xx_hal_gpio.h"

static color_t led_top_color[LED_TOP_LEN];
static color_t led_left_color[LED_LEFT_LEN];
static color_t led_right_color[LED_RIGHT_LEN];

//b_addr, b_pin, g_addr, g_pin, r_addr, r_pin
static led_id_t led_top_id[16][6] = {
		{DRV4, 0, DRV4, 1, DRV4, 2},
		{DRV4, 3, DRV4, 4, DRV4, 5},
		{DRV4, 6, DRV4, 7, DRV4, 8},
		{DRV4, 9, DRV4, 10, DRV4, 11},
		{DRV4, 12, DRV4, 13, DRV4, 14},
		{DRV4, 15, DRV5, 0, DRV5, 1},
		{DRV5, 2, DRV5, 3, DRV5, 4},
		{DRV5, 5, DRV5, 6, DRV5, 7},
		{DRV5, 8, DRV5, 9, DRV5, 10},
		{DRV5, 11, DRV5, 12, DRV5, 13},
		{DRV5, 14, DRV5, 15, DRV6, 0 },
		{DRV6, 1, DRV6, 2, DRV6, 3},
		{DRV6, 4, DRV6, 5, DRV6, 6},
		{DRV6, 7, DRV6, 8, DRV6, 9},
		{DRV6, 10, DRV6, 11, DRV6, 12},
		{DRV6, 13, DRV6, 14, DRV6, 15}};

static led_id_t led_left_id[8][6] = {
		{DRV1, 5, DRV1, 6, DRV1, 7},
		{DRV1, 2, DRV1, 3, DRV1, 4},
		{DRV1, 8, DRV1, 0, DRV1, 1},
		{DRV1, 11, DRV1, 10, DRV1, 9},
		{DRV1, 14, DRV1, 13, DRV1, 12},
		{DRV2, 6, DRV2, 7, DRV1, 15},
		{DRV2, 3, DRV2, 4, DRV2, 5},
		{DRV2, 0, DRV2, 1, DRV2, 2}};

static led_id_t led_right_id[8][6] = {
		{DRV2, 13, DRV2, 14, DRV2, 15},
		{DRV2, 10, DRV2, 11, DRV2, 12},
		{DRV3, 0, DRV2, 8, DRV2, 9},
		{DRV3, 3, DRV3, 2, DRV3, 1},
		{DRV3, 6, DRV3, 5, DRV3, 4},
		{DRV3, 14, DRV3, 15, DRV3, 7},
		{DRV3, 11, DRV3, 12, DRV3, 13},
		{DRV3, 8, DRV3, 9, DRV3, 10}};

void vLedUpdateTask(void * pvParameters) {
	uint32_t func_tick = 0;
	uint32_t func_ms = 0 ;

	portTickType xLastWakeTime;
	xLastWakeTime = osKernelSysTick();

	ledInitAll();

//	for(int i = 0; i < 16; i++) {
//		led_top_color[i] =  COLOR_GREEN;
//	}
//	for(int i = 0; i < 8; i++) {
//		led_left_color[i] =  COLOR_BLUE;
//	}
//	for(int i = 0; i < 8; i++) {
//		led_right_color[i] =  COLOR_RED;
//	}
//	color_t color_green = COLOR_GREEN;
//	int8_t len = LED_TOP_LEN;

	led_set_bar(50, COLOR_GREEN, led_top_color, LED_TOP_LEN);
	led_set_bar(30, COLOR_BLUE, led_right_color, LED_RIGHT_LEN);
	led_set_bar(70, COLOR_RED, led_left_color, LED_LEFT_LEN);


	for( ;; ) {
		//TODO: add error override
		//TODO: read CAN bus to display gradients

		led_set_bar((func_ms/25)%5000, COLOR_GREEN, led_top_color, LED_TOP_LEN);
		led_set_bar((func_ms/25)%5000, COLOR_BLUE, led_right_color, LED_RIGHT_LEN);
		led_set_bar((func_ms/25)%5000, COLOR_RED, led_left_color, LED_LEFT_LEN);

		for(int i = 0; i < 16; i++) {
			set_led(led_top_id[i], led_top_color[i]);
		}
		for(int i = 0; i < 8; i++) {
				set_led(led_left_id[i], led_left_color[i]);
		}
		for(int i = 0; i < 8; i++) {
				set_led(led_right_id[i], led_right_color[i]);
		}

		func_tick = func_tick+1;
		func_ms = (func_tick*10);


		osDelayUntil( &xLastWakeTime, ( osKernelSysTickMicroSec(period) ) );
	}
}



HAL_StatusTypeDef set_led(led_id_t * led_id, color_t led_color) {
	HAL_StatusTypeDef I2C_status;

	int8_t b_addr = led_id[0];
	int8_t b_pin = led_id[1];
	int8_t g_addr = led_id[2];
	int8_t g_pin = led_id[3];
	int8_t r_addr = led_id[4];
	int8_t r_pin = led_id[5];

	int32_t alpha = get_alpha(led_color);
	uint8_t red[2] = {r_pin + LED_DRV_PWM_REG_OFFSET, (get_red(led_color) * alpha)>>8};
	uint8_t blue[2] = {b_pin + LED_DRV_PWM_REG_OFFSET, (get_blue(led_color) * alpha)>>8};
	uint8_t green[2] = {g_pin + LED_DRV_PWM_REG_OFFSET,(get_green(led_color) * alpha)>>8};


	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, b_addr, blue, sizeof(int8_t)*2, I2C_TIMEOUT);
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, g_addr, green, sizeof(int8_t)*2, I2C_TIMEOUT);
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, r_addr, red, sizeof(int8_t)*2, I2C_TIMEOUT);

	return I2C_status;

}


void ledInitAll() {
	ledInit(DRV1);
	ledInit(DRV2);
	ledInit(DRV3);
	ledInit(DRV4);
	ledInit(DRV5);
	ledInit(DRV6);
	HAL_GPIO_WritePin(GPIOE, LED_DRV_OE_L_Pin, GPIO_PIN_RESET);
}

HAL_StatusTypeDef ledInit(int8_t led_addr) {
	HAL_StatusTypeDef I2C_status;

	//0h00 MODE1 = 0000 0000
	uint8_t MODE1_val[2] = {0x00, 0b00000000};
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, led_addr, MODE1_val, 2 , I2C_TIMEOUT);

	//0h01 MODE2 = 0000 0010 (open drain)
	uint8_t MODE2_val[2] = {0x01, 0b00000010};
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, led_addr, MODE2_val, 2, I2C_TIMEOUT);

	/* [0h14..0h17] LEDOUT[0..3] = 1010 1010
	 * LDRx = 10,  LED driver x individual brightness can be controlled through its PWMx
	 * register
	 */
	uint8_t LEDOUT0_val[2] = {0x14, 0b10101010};
	uint8_t LEDOUT1_val[2] = {0x15, 0b10101010};
	uint8_t LEDOUT2_val[2] = {0x16, 0b10101010};
	uint8_t LEDOUT3_val[2] = {0x17, 0b10101010};
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, led_addr, LEDOUT0_val, 2, I2C_TIMEOUT);
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, led_addr, LEDOUT1_val, 2, I2C_TIMEOUT);
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, led_addr, LEDOUT2_val, 2, I2C_TIMEOUT);
	I2C_status = HAL_I2C_Master_Transmit(&hi2c3, led_addr, LEDOUT3_val, 2, I2C_TIMEOUT);

	return I2C_status;
}

void led_set_bar(int8_t percentage, color_t bar_color, color_t * led_bar, int8_t led_bar_len) {
	percentage = percentage %101;

	int8_t i;
	int8_t midpoint = (int8_t) (((int32_t) led_bar_len * (int32_t) percentage)/100);
	for(i = 0; i <= midpoint; i++) {
		led_bar[i] = bar_color;
	}
	for(; i < led_bar_len; i++) {
		led_bar[i] = set_alpha(bar_color, 0);
	}
}
