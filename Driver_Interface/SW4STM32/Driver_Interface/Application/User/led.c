#include "led.h"

//static led_t led_bar_top[16];
//led_bar_top[0].g_d = 4;
//led_bar_top[0].g_p = 0;
//led_bar_top[0].g_d = 4;
//led_bar_top[0].g_p = 1;
//led_bar_top[0].g_p
void vLedUpdateTask(void * pvParameters) {

	portTickType xLastWakeTime;
	xLastWakeTime = osKernelSysTick();

	for( ;; ) {
		/* Look at memory pool from state task and see if any errors are true (BSPD_ERR, IMD_ERR ...)
		Look at msg from GUI to determine what to display, then
		populate memory with values of each LED address,
		then control LEDs via I2C*/


		osDelayUntil( &xLastWakeTime, ( osKernelSysTickMicroSec(period) ) );
	}
}

void set_led(led_t sel_led, color_t led_color) {
	//TODO: check this division works
	int32_t alpha = get_alpha(led_color);
	int8_t red[2] = {sel_led.r_p, (get_red(led_color) * alpha)>>8};
	int8_t blue[2] = {sel_led.b_p, (get_blue(led_color) * alpha)>>8};
	int8_t green[2] = {sel_led.g_p,(get_green(led_color) * alpha)>>8};


	HAL_I2C_Master_Transmit(i2c_handler, sel_led.b_a, blue, sizeof(int8_t)*2 I2C_TIMEOUT);
	HAL_I2C_Master_Transmit(i2c_handler, sel_led.g_a, green, sizeof(int8_t)*2, I2C_TIMEOUT);
	HAL_I2C_Master_Transmit(i2c_handler, sel_led.r_a, red, sizeof(int8_t)*2, I2C_TIMEOUT);
}


void ledInitAll() {
	//run ledInit across all leds
}

void ledInit(int LedNum) {

}
