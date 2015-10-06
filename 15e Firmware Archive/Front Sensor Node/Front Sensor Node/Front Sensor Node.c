/*
 * Front Sensor Node.c
 * The main file for the front sensor node. Samples sensors, checks TPS, 
 * heartbeats & state transitions. Broadcasts sensor readings on the
 * CAN network.
 * 
 * Created: 3/23/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATmega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 *	- Needs watchdog timers
 *	- Needs SPI and UART
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include <avr/power.h>
#include "init.h"
#include "cmr_library_64c1.h"
#include "can_lib.h"
#include "config.h"

/* Main code for Front Sensor Node
 * Author: Shepard Emerson
 * Last Edit: 4/9/15
 * 
 * Burned fuses: CLKSEL: 0100, SUT: 11
 * 
 */

// Current State
uint8_t state = GLV_ON;	// Initialize to GLV_ON
// Current throttle position
uint8_t throttle_pos = 0;
// Error
uint8_t error_flag = 0;

// Global array to store ADC sample values
uint8_t adc_values[14];

// ADC and SPI sampling interrupt handler, fires at 2ms, 12ms, 22ms, etc
// Use this interrupt for anything you need to do before the main interrupt, i.e. sample things
ISR(TIMER1_COMPB_vect) {
	// Current sampled channel
	int ch = 0;
	
	// Sample all ADC channels
	for(ch = 0; ch < 14; ch++) {
		adc_values[ch] = adc_read(ch);
	}
}

// Main 100Hz interrupt handler, fires at 10ms, 20ms, 30ms, etc
// Use this interrupt for any main things you need to do at 100Hz
ISR(TIMER1_COMPA_vect) { 
	// Function Variables
	uint16_t tps_percent_diff;
	
	// Reset the watchdog timer
	wdt_reset();
	
	// Reset counter
	TCNT1 = 0x0000;
	
	// Turn on MCU status pin, brightness indicates utilization level (PWM)
	// 15e_todo: comment this out if you don't have an MCU status LED
	set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, HIGH);
	
	// 15e_todo: FILL IN THE BODY OF YOUR INTERRUPT
	
	// Send sensor values over CAN
	/*can_send_buffer[0] = adc_values[DAMPER_POSITION_LEFT];
	can_tx_sync(DAMPER_POSITION_LEFT_ID, can_send_buffer, 1);
	can_send_buffer[0] = adc_values[DAMPER_POSITION_RIGHT];
	can_tx_sync(DAMPER_POSITION_RIGHT_ID, can_send_buffer, 1);
	can_send_buffer[0] = adc_values[CURRENT];
	can_tx_sync(CURRENT_ID, can_send_buffer, 1);
	can_send_buffer[0] = adc_values[STEERING_WHEEL_ANGLE];
	can_tx_sync(STEERING_WHEEL_ANGLE_ID, can_send_buffer, 1);
	can_send_buffer[0] = adc_values[BRAKE_PRESSURE];
	can_tx_sync(BRAKE_PRESSURE_ID, can_send_buffer, 1);
	can_send_buffer[0] = adc_values[THROTTLE_POSITION_L];
	can_tx_sync(THROTTLE_POSITION_L_ID, can_send_buffer, 1);
	can_send_buffer[0] = adc_values[THROTTLE_POSITION_R];
	can_tx_sync(THROTTLE_POSITION_R_ID, can_send_buffer, 1);*/
	
	// Clear error flag
	error_flag = 0;
	
	// Switch on the current state
	switch(state) {
		case GLV_ON:
			// Grounded low-voltage on
			// 15e_todo: Fill in code for GLV on here
			
			// Set throttle position to zero
			throttle_pos = 0;
			
			break;
			
		case HV_EN:
			// High-voltage enable
			// 15e_todo: Fill in code for HV_EN here
			
			// Set throttle position to zero
			throttle_pos = 0;
			
			break;
			
		case RTD:
			// Ready to drive
			// 15e_todo: Fill in code for RTD here
			
			// Calculate % diff of TPS1 and TPS2
			if(adc_values[THROTTLE_POSITION_L] > adc_values[THROTTLE_POSITION_R]) {
				// Percent diff = (difference / average) * 100
				tps_percent_diff = (uint16_t)adc_values[THROTTLE_POSITION_L] 
					- (uint16_t)adc_values[THROTTLE_POSITION_R];	// Difference
				tps_percent_diff = tps_percent_diff * 200;			// Multiply by 200 (move two from average to top)
				tps_percent_diff = tps_percent_diff
					/ (adc_values[THROTTLE_POSITION_L] + adc_values[THROTTLE_POSITION_R]);	// Divide by sum
				
			} else {
				// Percent diff = (difference / average) * 100
				tps_percent_diff = (uint16_t)adc_values[THROTTLE_POSITION_R]
				- (uint16_t)adc_values[THROTTLE_POSITION_L];	// Difference
				tps_percent_diff = tps_percent_diff * 200;			// Multiply by 200 (move two from average to top)
				tps_percent_diff = tps_percent_diff
				/ (adc_values[THROTTLE_POSITION_R] + adc_values[THROTTLE_POSITION_L]);	// Divide by sum
			}
			
			// See if within 10%
			if(tps_percent_diff <= 10) {
				// If so, set to throttle_position_l
				throttle_pos = adc_values[THROTTLE_POSITION_L];
			} else {
				// Otherwise, zero throttle position
				throttle_pos = 0;
				// and set error flag
				error_flag = 0;
			}

			break;
			
		case ERROR:
			// Error state
			// 15e_todo: Fill in code for ERROR here
			
			// Set throttle position to zero
			throttle_pos = 0;
			
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
	
	// Transmit heartbeat
	// Fill in heartbeat data, current state
	// Pack FSN heartbeat struct
	fsn_heartbeat.state = state;
	fsn_heartbeat.bps = adc_values[BRAKE_PRESSURE];
	fsn_heartbeat.tps = throttle_pos;
	fsn_heartbeat.error = error_flag;
	
	// Transmit over CAN
	can_tx_sync(FSN_HEARTBEAT_ID, (uint8_t *)(&fsn_heartbeat), sizeof(fsn_heartbeat));
	
	// Turn off MCU Status LED
	// 15e_todo: comment this out if you don't have an MCU status LED
	set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, LOW);
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
	init_can_mailboxes();
	
	// Initialize ADC and 100Hz ADC sampling interrupt.
	// 15e_todo: Comment this out if you do not use ADC. This should be called before init_interrupts
	// if you use ADC
	init_adc();
	
	// Initialize watchdog timer
	// init_watchdog();
	
	// Initialize main timer output compare interrupt (100Hz) and enable interrupts
	init_interrupts();
	
	// Loop forever
	/* @non-terminating@ */
	while(1);
}