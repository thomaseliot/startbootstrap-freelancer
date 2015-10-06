/*
 * Rear Sensor Node.c
 * The main file for the front sensor node. Samples sensors, checks TPS, 
 * heartbeats & state transitions. Broadcasts sensor readings on the
 * CAN network.
 * 
 * Created: 3/23/2015
 * Author: Daniel Haddox (dhaddox)
 * Platform: ATmega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/power.h>
#include "init.h"
#include "cmr_library_64c1.h"
#include "can_lib.h"
#include "config.h"
#include "cooling.h"

/* Main code for Front Sensor Node
 * Author: Daniel Haddox
 * Last Edit: 4/16/15
 * 
 * Burned fuses: CLKSEL: 0100, SUT: 11
 * 
 */

// Current State
uint8_t state = GLV_ON;	// Initialize to GLV_ON

// Global array to store ADC sample values
uint8_t adc_values[14];

//Global variable for brake light status
uint8_t brake_pressure_val;




// ADC and SPI sampling interrupt handler, fires at 2ms, 12ms, 22ms, etc
// Use this interrupt for anything you need to do before the main interrupt, i.e. sample things
ISR(TIMER1_COMPB_vect) {
	// Current sampled channel
	int ch = 2;
	// Grounded low-voltage on
	// 15e_todo: Fill in code for GLV on here
			
	// Sample all ADC channels, 2 through 5
	for(ch = 2; ch < 6; ch++) {
		adc_values[ch-2] = adc_read(ch);
	}
		
}



// Main 100Hz interrupt handler, fires at 10ms, 20ms, 30ms, etc
// Use this interrupt for any main things you need to do at 100Hz
ISR(TIMER1_COMPA_vect) {
	// Function Variables
	
	// Reset the watchdog timer
	wdt_reset();
	
	// Reset counter for main interrupt
	// DO NOT REMOVE THIS LINE
	TCNT1 = 0x0000;
	
	// Turn on MCU status pin, brightness indicates utilization level (PWM)
	// 15e_todo_done: comment this out if you don't have an MCU status LED
	set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, HIGH);
	
	// 15e_todo: FILL IN THE BODY OF YOUR INTERRUPT
// 	
// 	//Send ADC channels via CAN
// 				
// 	// Send all sensor values, Channels 0 and 1 are not used so offset
// 	can_send_buffer[0] = adc_values[WTEMP_L_CH-2];
// 	can_tx_sync(WTEMP_L_CH_ID, can_send_buffer, 1);
// 	can_send_buffer[0] = adc_values[WTEMP_R_CH-2];
// 	can_tx_sync(WTEMP_R_CH_ID, can_send_buffer, 1);
// 	can_send_buffer[0] = adc_values[SLINEAR_L_CH-2];
// 	can_tx_sync(SLINEAR_L_CH_ID, can_send_buffer, 1);
// 	can_send_buffer[0] = adc_values[SLINEAR_R_CH-2];
// 	can_tx_sync(SLINEAR_R_CH_ID, can_send_buffer, 1);

	
	// Switch on the current state
	switch(state) {
		//fan_speed = get_smart_fan_speed(); 15e_todo update fan speed here for non-barebones build
		case GLV_ON:
			// Grounded low-voltage on
			// 15e_todo: Fill in code for GLV on here
			update_cooling(COOLING_DISABLE, GLV_ON_FAN_SPEED);
			break;
			
		case HV_EN:
			// High-voltage enable
			// 15e_todo: Fill in code for HV_EN here
			update_cooling(COOLING_ENABLE,HV_EN_FAN_SPEED);
			break;
			
		case RTD:
			// Ready to drive
			// 15e_todo: Fill in code for RTD here
			update_cooling(COOLING_ENABLE,RTD_FAN_SPEED);
			break;
			
		case ERROR:
			// Error state
			// 15e_todo: Fill in code for ERROR here
			update_cooling(COOLING_DISABLE,ERROR_FAN_SPEED);
			break;
	}
	
		
	/* Check mailboxes for Main Board Heartbeat message
	 *	and transition states if necessary
	 */
	if(can_get_status(&rcv_cmd_st[0]) != CAN_STATUS_NOT_COMPLETED) {
		// There is, update the state
		state = ((struct main_heartbeat_st *)(&can_rcv_buffer[0]))->state;
		/* Setup mailbox to receive Main board state again
		 *		Necessary since the mailbox is cleared after a message is received
		 */
		while(can_cmd(&rcv_cmd_st[0]) != CAN_CMD_ACCEPTED);
	}
	
	/* Check mailboxes for FSN Board Heartbeat message
	 *	and update brake light status
	 */
	if(can_get_status(&rcv_cmd_st[1]) != CAN_STATUS_NOT_COMPLETED) {
		// There is, update the state
		brake_pressure_val = ((struct fsn_heartbeat_st *)(&can_rcv_buffer[1]))->bps;
		/* Setup mailbox to receive Main board state again
		 *		Necessary since the mailbox is cleared after a message is received
		 */
		while(can_cmd(&rcv_cmd_st[1]) != CAN_CMD_ACCEPTED);
	}
	
	
	
	
	// Transmit heartbeat
	// Fill in heartbeat data, current state
	// Pack RSN heartbeat struct
	rsn_heartbeat.state = state;
	
	// Transmit over CAN
	can_tx_sync(RSN_HEARTBEAT_ID, (uint8_t *)(&rsn_heartbeat), sizeof(rsn_heartbeat));
	
	// Turn off MCU Status LED
	// Turn off MCU Status LED
	// 15e_todo_done: comment this out if you don't have an MCU status LED
	set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, LOW);
	
	//Turn brake light on or off
	if (brake_pressure_val > BRAKE_THRESH){
		set_io_pin(BRAKE_CTRL_PORT, BRAKE_CTRL_PIN, HIGH);
	}
	else {
		set_io_pin(BRAKE_CTRL_PORT, BRAKE_CTRL_PIN, LOW);
	}
	

}

/* Main loop, initializes CPU and enables interrupts then loops forever.
 * Interrupts will fire to do useful things at a specified rate (100Hz in this case)
 */
int main(void)
{
	// Set the clock prescaler to be 1 (full clock rate)
	clock_prescale_set(clock_div_1);
	
	// Initialize all board I/O, including cooling control
	init_io();
	
	// Initialize CAN, fixed baudrate defined in config.h (passing 0 does autobaud)
	can_init(1);	// Fixed baud rate
	
	// Initialize CAN mailboxes 
	init_can_mailboxes();
	
	// Initialize ADC and 100Hz ADC sampling interrupt.
	// 15e_todo_done: Comment this out if you do not use ADC. This should be called before init_interrupts
	// if you use ADC
	init_adc();
	
	// Initialize main timer output compare interrupt (100Hz) and enable interrupts
	init_interrupts();
	
	// Loop forever
	/* @non-terminating@ */
	while(1);
}