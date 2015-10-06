/*
 * init.c
 * 
 * Board-specific initialization functions
 * 
 * Created: 3/23/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATmega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson
 * 
 * Known Errors:
 *	- None
 */

#include <stdio.h>
#include "init.h"
#include "avr/interrupt.h"
#include "cmr_library_64c1.h"

//Define CAN mailboxes
st_cmd_t heartbeat_cmd_st; // Receive command for 1 reserved heartbeat mailbox
st_cmd_t rcv_cmd_st[5];	// Five receive commands for remaining five mailboxes
//Setup buffer to receive into
uint64_t can_rcv_buffer[5];
uint64_t can_send_buffer;

/* Declare struct to send RSN heartbeat messages */
struct main_heartbeat_st main_heartbeat;
 
/* init_interrupts
 * Initialize interrupts and enable the 100Hz main loop output compare timer interrupt.
 * Arguments: void
 * Returns: void
 */
void init_interrupts(void) {
	
	//Enable output compare A interrupt for Timer 1 (enables interrupt)
	TIMSK1 |= _BV(OCIE1A);
	
	// Set timer1_clk prescaler to clk/256 = 62.5kHz with 16MHz system clock
	TCCR1A = _BV(WGM12);
	TCCR1B = _BV(CS12) | _BV(CS11);
	
	// Reset timer 1
	TCNT1 = 0;

	// Set timer1_clk prescaler to clk/256 = 62.5kHz with 16MHz system clock
	TCCR1A = 0;
	TCCR1B &= (1 << CS12) | (0 << CS11) | (0 << CS10); 
	
	/*Set output compare register 1A to 0x271=625
		This sets the timer to count to 625, making a
		100Hz interrupt
	*/
	OCR1A = 0x0271;
	
	// Clear interrupts
	TIFR1 = (1 << OCF1A) || (1 << OCF1B);

	// enable interrupts
	sei();
}

/* init_io
 * Initialize digital I/O pins.
 * Arguments: void
 * Returns: void
 */
void init_io(void) {
	config_io_pin(MCU_STATUS_PIN, MCU_STATUS_PORT, IO_DIR_OUTPUT);
}

/* init_io
 * Initialize analog to digital converter and pins.
 * Arguments: void
 * Returns: void
 */
void init_adc(void){

	ADCSRA |= (1 << ADPS2) | (0 << ADPS1) | (0 << ADPS0); // Set ADC prescaler to 16 - 1MHz sample rate @ 16MHz

	ADMUX  |= (1 << ADLAR); // Left adjust ADC result to allow easy 8 bit reading
	ADCSRB |= (1 << ADHSM);	// Set ADC high speed mode

	// No MUX values needed to be changed for initialization
	
	ADCSRB |= (1 << AREFEN); // Set ADC reference to AREF (AREFEN = 1, ISRCEN = 0, REFS1 = 0, REFS0 = 0)
	
	ADCSRA |= (1 << ADEN);  // Enable ADC
	
	// Enable timer 1 output compare B interrupt to sample ADCs
	// 62.5KHz timer clock * 2.5ms compare time = 156 = 0x9C
	OCR1B = 0x009C;
	// Enable output compare B interrupt
	TIMSK1 |= _BV(OCIE1B);
}

/* init_watchdog
 * Initialize the watchdog timer and start it. 120ms timeout.
 * Arguments: void
 * Returns: void
 */
void init_watchdog(void) {
	// Disable watchdog timer
	wdt_disable();
	
	// Enable the watchdog timer, 120ms timeout
	wdt_enable(WDTO_120MS);
}

/* init_can_mailboxes
 * sets up mailboxes for all critical CAN messages that this board receives
 *
 */
void init_can_mailboxes(void) {
	// Initialize heartbeat send mailbox for Main Board
	setup_can_send(&heartbeat_cmd_st, ((uint8_t *)(&can_send_buffer)), sizeof(struct main_heartbeat_st), MAIN_HEARTBEAT_ID);
	
	/* Initialize receive mailboxes    */
	// Receive heartbeats from all nodes
	setup_can_rcv(&rcv_cmd_st[0], ((uint8_t *)(&can_rcv_buffer[0])), sizeof(struct fsn_heartbeat_st), FSN_HEARTBEAT_ID);
	setup_can_rcv(&rcv_cmd_st[1], ((uint8_t *)(&can_rcv_buffer[1])), sizeof(struct rsn_heartbeat_st), RSN_HEARTBEAT_ID);
	setup_can_rcv(&rcv_cmd_st[2], ((uint8_t *)(&can_rcv_buffer[2])), sizeof(struct dash_heartbeat_st), DASH_HEARTBEAT_ID);
}