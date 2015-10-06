//#define F_CPU 2000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/power.h>
#include "init.h"
#include "cmr_library_64c1.h"
#include "can_lib.h"
#include "config.h"
#include "dashboard_dio.h"
#include "standard_dio.h"
#include "spi.h"
#include "display.h"

/* Main code for Dashboard
 * 
 * Burned fuses: CLKSEL: 0100, SUT: 11 
 *
 * Barebones Requirements:
 *  - Send button presses
 *  - Display state
 *  - GLV Error LED 
 *  - BSPD, IMD, BMS error LEDs
 *  - RTD sound
 *  
 */

// Current State
uint8_t state = GLV_ON;	     // Initialize to GLV_ON

// global variables for dashboard functions
uint8_t dummy_data = 0;          // dummy byte to use for sending CAN messages
uint8_t HVE_btn_state = 0;       // current state of high voltage enable button
uint8_t prev_HVE_btn_state = 0;  // previous state of high voltage enable button
uint8_t RTD_btn_state = 0;       // current state of ready to drive button
uint8_t prev_RTD_btn_state = 0;  // previous state of ready to drive button
uint8_t reset_btn_state = 0;       // current state of reset button
uint8_t prev_reset_btn_state = 0;  // previous state of reset button
uint8_t DAQ_btn_state = 0;       // current state of DAQ button
uint8_t prev_DAQ_btn_state = 0;  // previous state of DAQ button

// dashboard delays (seconds * 100)
#define RTD_buzzer_duration 300
#define display_update_duration 20
#define button_debounce_delay 50

// delay cycle counters
uint16_t RTD_state_counter = 0;
uint16_t disp_counter = 0;
uint16_t button_debounce_counter = 0;

// transmission bits
char send_HVE_btn = 0;
char send_RTD_btn = 0;
char send_reset_btn = 0;
char send_daq_btn = 0;

#define accept_button_press (button_debounce_counter >= button_debounce_delay)

// ADC and SPI sampling interrupt handler, fires at 2ms, 12ms, 22ms, etc
// Use this interrupt for anything you need to do before the main interrupt, i.e. sample things
ISR(TIMER1_COMPB_vect) {
	// update button states
	prev_HVE_btn_state = HVE_btn_state;
	prev_RTD_btn_state = RTD_btn_state;
	prev_reset_btn_state = reset_btn_state;
	prev_DAQ_btn_state = DAQ_btn_state;
	
	HVE_btn_state = HVE_btn_pressed();
	RTD_btn_state = RTD_btn_pressed();
	reset_btn_state = reset_btn_pressed();
	DAQ_btn_state = DAQ_btn_pressed();
	
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
	//display_int(1, RTD_counter, 0b001, 0b1010);
	
	// Reset the watchdog timer
	//wdt_reset();
	
	// Reset counter
	TCNT1 = 0x0000;

		
	// count the time since the last button was released
	if (button_debounce_counter < button_debounce_delay && !HVE_btn_state && !RTD_btn_state && !reset_btn_state && !DAQ_btn_state) {
		button_debounce_counter++;
	}
	
	send_HVE_btn = 0;
	send_RTD_btn = 0;
	send_reset_btn = 0;
	
	// Check buttons and transmit if any are newly pressed
	if (HVE_btn_state && !prev_HVE_btn_state && accept_button_press) {
		// HVE button was pushed
		button_debounce_counter = 0;
		send_HVE_btn = 1;
	}
	if (RTD_btn_state && !prev_RTD_btn_state && accept_button_press) {
		// RTD button was pushed
		button_debounce_counter = 0;
		send_RTD_btn = 1;
	}
	if (reset_btn_state && !prev_reset_btn_state && accept_button_press) {
		// reset button was pushed
		button_debounce_counter = 0;
		send_reset_btn = 1;
	}
	if (DAQ_btn_state && !prev_DAQ_btn_state && accept_button_press) {
		// DAQ button was pushed (latches in)
		button_debounce_counter = 0;
		send_daq_btn = 1;
		DAQ_btn_illum(1);
	}
	if (!DAQ_btn_state && prev_DAQ_btn_state && accept_button_press) {
		// DAQ button was released
		button_debounce_counter = 0;
		send_daq_btn = 0;
		DAQ_btn_illum(0);
	}
	
	// Switch on the current state
	switch(state) {
		case GLV_ON:
			// Grounded low-voltage on
			HVE_btn_illum(0);
			RTD_btn_illum(0);
			reset_btn_illum(1);
			ERROR_LED(0);
			RTD_buzzer(0);
			RTD_state_counter = 0;
			// display state on display
			if (disp_counter >= display_update_duration) {
				display_str(1,"GLV",0b011,0b0000);
				display_str(3,"on",0b011,0b0000);
				disp_counter = 0;
			} else {
				disp_counter++;
			}
			break;
			
		case HV_EN:
			// High-voltage enable
			HVE_btn_illum(1);
			RTD_btn_illum(0);
			reset_btn_illum(1);
			ERROR_LED(0);
			RTD_buzzer(0);
			RTD_state_counter = 0;
			// display state on display
			if (disp_counter >= display_update_duration) {
				display_str(1,"HV",0b110,0b0000);
				display_str(3,"on",0b110,0b0000);
				disp_counter = 0;
			} else {
				disp_counter++;
			}
			break;
			
		case RTD:
			// Ready to drive
			HVE_btn_illum(1);
			RTD_btn_illum(1);
			reset_btn_illum(1);
			ERROR_LED(0);
			if (RTD_state_counter < RTD_buzzer_duration) {
				RTD_buzzer(1);
				RTD_state_counter++;
			} else {
				RTD_buzzer(0);
			}
			// display state on display
			if (disp_counter >= display_update_duration) {
				display_str(1,"rEA",0b010,0b0000);
				display_str(3,"dy",0b010,0b0000);
				disp_counter = 0;
			} else {
				disp_counter++;
			}
			break;
			
		case ERROR:
			// Error state
			HVE_btn_illum(0);
			RTD_btn_illum(0);
			reset_btn_illum(0);
			ERROR_LED(1);
			RTD_buzzer(0);
			RTD_state_counter = 0;
			// display state on display
			if (disp_counter >= display_update_duration) {
				display_str(1,"Err",0b100,0b1111);
				display_str(3,"or",0b100,0b1111);
				disp_counter = 0;
			} else {
				disp_counter++;
			} 
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
	
	// fake state transitions for testing
	//if (state == GLV_ON && HVE_btn_state && accept_button_press) {
		//state = HV_EN;
		//button_debounce_counter = 0;
	//}
	//if (state == HV_EN && RTD_btn_state && accept_button_press) {
		//state = RTD;
		//button_debounce_counter = 0;
	//}
	//if (state == HV_EN && HVE_btn_state && accept_button_press) {
		//state = GLV_ON;
		//button_debounce_counter = 0;
	//}
	//if (state == RTD && RTD_btn_state && accept_button_press) {
		//state = HV_EN;
		//button_debounce_counter = 0;
	//}
	//if (state != ERROR && mode_btn_pressed() && accept_button_press) {
		//state = ERROR;
		//button_debounce_counter = 0;
	//}
	//if (state == ERROR && reset_btn_state && accept_button_press) {
		//state = GLV_ON;
		//button_debounce_counter = 0;
	//}
	
	// Transmit heartbeat
	// Fill in heartbeat data, current state
	// Pack FSN heartbeat struct
	dash_heartbeat.state = state;
	dash_heartbeat.hv_en = send_HVE_btn;
	dash_heartbeat.rtd = send_RTD_btn;
	dash_heartbeat.clr_err = send_reset_btn;
	dash_heartbeat.daq_en = send_daq_btn;
	
	// Transmit over CAN
	can_tx_sync(DASH_HEARTBEAT_ID, (uint8_t *)(&dash_heartbeat), sizeof(dash_heartbeat));
	
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
	
	init_adc();
	
	// Initialize watchdog timer
	//init_watchdog();
	
	// Initialize main timer output compare interrupt (100Hz) and enable interrupts
	init_interrupts();
	
	// Loop forever
	/* @non-terminating@ */
	while(1);
}