/*
 * tasks.c
 *
 * Created: 11/23/2015 6:19:47 PM
 *  Author: skir0
 */ 

#include "node_tasks.h"
#include <avr/io.h>

void config_io_pin(uint8_t port, uint8_t port_ch, uint8_t dir){
	switch(port){
		case IO_PORT_B:
		//Crazy bit trickery that sets the port_ch bit of DDRB to dir
		DDRB ^= ((-dir) ^ DDRB) & (1 << port_ch);
		break;
		case IO_PORT_C:
		DDRC ^= ((-dir) ^ DDRC) & (1 << port_ch);
		break;
		case IO_PORT_D:
		DDRD ^= ((-dir) ^ DDRD) & (1 << port_ch);
		break;
		default:
		break;
	}
}

void set_io_pin(uint8_t port, uint8_t port_ch, uint8_t val) {
	switch(port){
		case IO_PORT_B:
		PORTB ^= ((-val) ^ PORTB) & (1 << port_ch);
		break;
		case IO_PORT_C:
		PORTC ^= ((-val) ^ PORTC) & (1 << port_ch);
		break;
		case IO_PORT_D:
		PORTD ^= ((-val) ^ PORTD) & (1 << port_ch);
		break;
		default:
		break;
	}
}

void vLEDFlashTask(void *pvParameters) {
	
	for(;;) {
		PORTC ^= (1 << 0);
		vTaskDelay(1000);
	}
}