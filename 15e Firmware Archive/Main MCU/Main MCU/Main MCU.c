/*
 * Main MCU.c
 * The main file for the front sensor node. Acts as master node in the system.
 * 
 * Created: 3/23/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATmega64C1
 * Modified: 4/29/2015
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

// Current State
uint8_t state = GLV_ON;	// Initialize to GLV_ON

// Current torque requested
uint8_t torque = 0;

// Node incorrect state counts
uint8_t rsn_wrong_state = 0;
uint8_t fsn_wrong_state = 0;
uint8_t dash_wrong_state = 0;

// Structs for sensor heartbeat messages
struct fsn_heartbeat_st fsn_heartbeat;
struct rsn_heartbeat_st rsn_heartbeat;
struct dash_heartbeat_st dash_heartbeat;

// ADC and SPI sampling interrupt handler, fires at 2ms, 12ms, 22ms, etc
// Use this interrupt for anything you need to do before the main interrupt, i.e. sample things
ISR(TIMER1_COMPB_vect) {
	// Do nothing
}

// Main 100Hz interrupt handler, fires at 10ms, 20ms, 30ms, etc
// Use this interrupt for any main things you need to do at 100Hz
ISR(TIMER1_COMPA_vect) { 
	/* Function variables */
	
	// Timeout for wrong states
	uint8_t system_error = 0;
	
	// Reset the watchdog timer
	wdt_reset();
	
	// Reset counter
	TCNT1 = 0x0000;
	
	// Turn on MCU status pin, brightness indicates utilization level (PWM)
	// 15e_todo: comment this out if you don't have an MCU status LED
	set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, HIGH);
	
	// 15e_todo: FILL IN THE BODY OF YOUR INTERRUPT
	
	/* Check mailboxes for heartbeat messages
	 * and make necessary state/variable changes
	 */
	// Front sensor node
	if(can_get_status(&rcv_cmd_st[0]) != CAN_STATUS_NOT_COMPLETED) {
		// There is, get the state
		fsn_heartbeat = *((struct fsn_heartbeat_st *)(&can_rcv_buffer[0]));
		
		// See if state is incorrect
		if(fsn_heartbeat.state != state) fsn_wrong_state++;	// If it is, increment wrong state count
		else fsn_wrong_state = 0;								// Otherwise reset wrong state count
		
		/* Setup mailbox to receive Main board state again
		 *		Necessary since the mailbox is cleared after a message is received
		 */
		while(can_cmd(&rcv_cmd_st[0]) != CAN_CMD_ACCEPTED);
	}
	
	// Rear sensor node
	if(can_get_status(&rcv_cmd_st[1]) != CAN_STATUS_NOT_COMPLETED) {
		// There is, get the state
		rsn_heartbeat = *((struct rsn_heartbeat_st *)(&can_rcv_buffer[1]));
		
		// See if state is incorrect
		if(rsn_heartbeat.state != state) rsn_wrong_state++;	// If it is, increment wrong state count
		else rsn_wrong_state = 0;								// Otherwise reset wrong state count
		
		/* Setup mailbox to receive Main board state again
		 *		Necessary since the mailbox is cleared after a message is received
		 */
		while(can_cmd(&rcv_cmd_st[1]) != CAN_CMD_ACCEPTED);
	}
	
	// Dashboard
	if(can_get_status(&rcv_cmd_st[2]) != CAN_STATUS_NOT_COMPLETED) {
		// There is, get the state
		dash_heartbeat = *((struct dash_heartbeat_st *)(&can_rcv_buffer[2]));
		
		// See if state is incorrect
		if(dash_heartbeat.state != state) dash_wrong_state++;	// If it is, increment wrong state count
		else dash_wrong_state = 0;								// Otherwise reset wrong state count
		
		// Get button positions
		
		/* Setup mailbox to receive Main board state again
		 *		Necessary since the mailbox is cleared after a message is received
		 */
		while(can_cmd(&rcv_cmd_st[2]) != CAN_CMD_ACCEPTED);
	}
	
	
	
	// See if any node has been in incorrect state for too long
	if(dash_wrong_state > 100 || fsn_wrong_state > 100 || rsn_wrong_state > 100) {
		// If it has been, go to error state
		system_error = 1;
	}
	
	// See if front sensor node has thrown any errors
	if(fsn_heartbeat.error) {
		system_error = ERROR;
	} 
	// Perform state transitions
	// Switch on the current state
	switch(state) {
		/*=========== GROUNDED LOW VOLTAGE ON ============*/
		case GLV_ON:
			// Transition to error state if any errors occurred
			if(system_error) {
				state = ERROR;
			}
			// Transition to HV Enable if button pressed
			else if(dash_heartbeat.hv_en) {
				state = HV_EN;	
			}
			break;
			
		/*============= HIGH VOLTAGE ENABLE ==============*/
		case HV_EN:		// High-voltage enable
			// Transition to error state if any errors occurred
			if(system_error) {
				state = ERROR;
			}
			// Transition to RTD if button pressed and brake pressed
			else if(dash_heartbeat.rtd && fsn_heartbeat.bps > BRAKE_THRESH) {
				state = RTD;
			}	
			// Transition to GLV_ON if HV_EN button pressed
			else if(dash_heartbeat.hv_en) {
				state = GLV_ON;
			}
			break;
		
		/*============== READY TO DRIVE ===============*/	
		case RTD:		// Ready to drive
			// Transition to error state if any errors occurred
			if(system_error) {
				state = ERROR;
			}
			// Transition to GLV_ON if HV_EN button pressed
			else if(dash_heartbeat.hv_en) {
				state = GLV_ON;
			}
			// Transition to HV_EN if RTD button pressed
			else if(dash_heartbeat.rtd) {
				state = HV_EN;
			}
			break;
		
		/*=================== ERROR ===================*/		
		case ERROR:		// Error 
			// Transition to error state if any errors occurred
			if(system_error) {
				state = ERROR;
			}
			// Transition to GLV_ON if clear error button pressed
			else if(dash_heartbeat.clr_err) {
				state = GLV_ON;
			}
			break;
		
		default:		// This should never happen
			state = ERROR;	// If this happens something bad happened
			break;
	}
	
	// Transmit heartbeat
	// Fill in heartbeat data, current state
	// Pack main heartbeat struct
	main_heartbeat.state = state;
	
	// Transmit over CAN
	can_tx_sync(MAIN_HEARTBEAT_ID, (uint8_t *)(&main_heartbeat), sizeof(main_heartbeat));

	// Reset heartbeat messages
	heartbeat_reset();

	// TODO: CALCULATE AND SEND TORQUE COMMAND TO MOTOR CONTROLLERS
	
	// Turn off MCU Status LED
	// 15e_todo: comment this out if you don't have an MCU status LED
	set_io_pin(MCU_STATUS_PORT, MCU_STATUS_PIN, LOW);
}

void heartbeat_reset(void) {
	// Initialize receive structs
	fsn_heartbeat.state = 0;
	fsn_heartbeat.bps = 0;
	fsn_heartbeat.tps = 0;
	fsn_heartbeat.error = 0;
	rsn_heartbeat.state = 0;
	dash_heartbeat.state = 0;
	dash_heartbeat.clr_err = 0;
	dash_heartbeat.daq_en = 0;
	dash_heartbeat.hv_en = 0;
	dash_heartbeat.rtd = 0;
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
	
	heartbeat_reset();
	
	// Initialize watchdog timer
	// init_watchdog();
	
	// Initialize main timer output compare interrupt (100Hz) and enable interrupts
	init_interrupts();
	
	// Loop forever
	/* @non-terminating@ */
	while(1);
}