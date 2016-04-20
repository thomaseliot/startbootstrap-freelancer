#include "led.h"
void vLedUpdateTask(void * pvParameters) {

	portTickType xLastWakeTime;
	xLastWakeTime = osKernelSysTick();

	for( ;; ) {
		/* Look at memory pool from state task and see if any errors are true (BSPD_ERR, IMD_ERR ...)
		Look at msg from GUI to determine what to display, then
		populate memory with values of each LED address,
		then control LEDs via I2C*/

		/* 16.667ms period = 60Hz */
		osDelayUntil( &xLastWakeTime, ( osKernelSysTickMicroSec(16667) ) );
	}
}
