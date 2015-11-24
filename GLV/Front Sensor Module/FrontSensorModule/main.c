/*
 * FrontSensorModule.c
 *
 * Created: 11/23/2015 5:51:36 PM
 * Author : skir0
 */ 

#include <avr/io.h>
#include "node_tasks.h"

/* Scheduler include files. */
#include "FreeRTOS.h"
#include "task.h"
#include "croutine.h"

#define mainLED_TASK_PRIORITY           ( tskIDLE_PRIORITY + 2 )

int main(void)
{
	config_io_pin(IO_PORT_C, 0, IO_DIR_OUTPUT);
	
    xTaskCreate(vLEDFlashTask, "LED", configMINIMAL_STACK_SIZE, 
		NULL, mainLED_TASK_PRIORITY, NULL);
	
	vTaskStartScheduler();
	
	return 0;
}

