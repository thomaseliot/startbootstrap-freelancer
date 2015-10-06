/*
 * DAQ Node.c
 * The main file for the DAQ board.  
 * 
 * Created: 4/19/2015
 * Author: Daniel Gorziglia (dgorzigl)
 * Platform: ATmega64C1
 * Modified: 4/19/2015
 * 
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/power.h>
#include "init.h"
#include "cmr_library_64c1.h"
#include "can_lib.h"
#include "config.h"

// Current State
uint8_t state = GLV_ON;	// Initialize to GLV_ON

// CAN variables
#define DATA_BUFFER_SIZE 8					// CAN send buffer size, 8 bytes max
#define STATE_BUFFER_SIZE 1					// CAN state receive buffer, 1 byte
uint8_t can_send_buffer[DATA_BUFFER_SIZE];	// CAN send buffer
uint8_t can_state_buffer[STATE_BUFFER_SIZE];	// CAN receive buffer
st_cmd_t can_send_msg;						// send command object
st_cmd_t can_receive_msg;					// receive command object

// ADC and SPI sampling interrupt handler, fires at 2ms, 12ms, 22ms, etc
// Use this interrupt for anything you need to do before the main interrupt, i.e. sample things
ISR(TIMER1_COMPB_vect) {
	// Switch on the current state
	switch(state) {
		case GLV_ON:
		// Grounded low-voltage on
		// 15e_todo: Fill in code for GLV on here
		break;
		
		case HV_EN:
		// High-voltage enable
		// 15e_todo: Fill in code for HV_EN here
		break;
		
		case RTD:
		// Ready to drive
		// 15e_todo: Fill in code for RTD here
		break;
		
		case ERROR:
		// Error state
		// 15e_todo: Fill in code for ERROR here
		break;
	}
}

// Main 100Hz interrupt handler, fires at 10ms, 20ms, 30ms, etc
// Use this interrupt for any main things you need to do at 100Hz
ISR(TIMER1_COMPA_vect) { 
	// Function Variables
	uint8_t cnt;	// Loop counter
	
	// Reset the watchdog timer
	wdt_reset();
	
	// Reset counter
	// DO NOT REMOVE THIS LINE
	TCNT1 = 0x0000;
	
	// Turn on MCU status pin, brightness indicates utilization level (PWM)
	// 15e_todo: comment this out if you don't have an MCU status LED
	// set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, HIGH);
	
	// 15e_todo: FILL IN THE BODY OF YOUR INTERRUPT
	
	// Switch on the current state
	switch(state) {
		case GLV_ON:
			// Grounded low-voltage on
			// 15e_todo: Fill in code for GLV on here
			break;
			
		case HV_EN:
			// High-voltage enable
			// 15e_todo: Fill in code for HV_EN here
			break;
			
		case RTD:
			// Ready to drive
			// 15e_todo: Fill in code for RTD here
			break;
			
		case ERROR:
			// Error state
			// 15e_todo: Fill in code for ERROR here
			break;
	}
	
	// STATE MACHINE THINGS. THIS MUST BE PRESENT
	// See if there is a new state message and transition state if necessary
	if(can_get_status(&can_receive_msg) != CAN_STATUS_NOT_COMPLETED) {
		// There is, update the state
		state = can_state_buffer[0];
		// Send receive command again
		while(can_cmd(&can_receive_msg) != CAN_CMD_ACCEPTED);
	}
	
	// Transmit heartbeat
	// Fill in heartbeat data, current state
	can_send_buffer[0] = state;
	
	// Transmit over CAN
	can_tx_sync(DAQ_HEARTBEAT_ID, can_send_buffer, 1);
	
	// Turn off MCU Status LED
	// 15e_todo: comment this out if you don't have an MCU status LED
	// set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, LOW);
}

/* Main loop, initializes CPU and enables interrupts then loops forever.
 * Interrupts will fire to do useful things at a specified rate (100Hz in this case)
 */
int main(void)
{
	// Set the clock prescaler to be 1 (full clock rate)
	clock_prescale_set(clock_div_1);
	
	// Initialize all board I/O
	init_io();
	
	// Initialize CAN, fixed baudrate defined in config.h (passing 0 does autobaud)
	can_init(1);	// Fixed baud rate
	
	// Initialize state mailbox
	init_state_capture(&can_receive_msg, can_state_buffer, STATE_BUFFER_SIZE);
	
	// Initialize watchdog timer
	// init_watchdog();
	
	// Initialize main timer output compare interrupt (100Hz) and enable interrupts
	init_interrupts();
	
	// Loop forever
	/* @non-terminating@ */
	while(1);
}