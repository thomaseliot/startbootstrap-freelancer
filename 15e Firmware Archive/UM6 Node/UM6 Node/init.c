/*
 * init.c
 * 
 * Board-specific initialization functions
 * 
 * Created: 3/23/2015
 * Author: Daniel Gorziglia (dgorzigl)
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
	
}

/* init_sample_interrupt
 * Initializes sampling interrupt at 100Hz
 * Arguments: void
 * Returns: void
 */
void init_sample_interrupt(void) {
	OCR1B = 0x009C; // Sets a 2.5ms output compare interrupt 
	
	TIMSK1 |= (1 << OCIE1B); // Enables output compare B interrupt
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

/* init_state_capture
 * Initialize CAN RX state buffer
 * Arguments:
 *  can_receive_msg: the message object
 *  *can_receive_buffer: the buffer
 *  BUF_LEN: length of the buffer
 */
void init_state_capture(st_cmd_t *can_receive_msg, uint8_t *can_receive_buffer, uint8_t BUF_LEN) {
	// Populate message object
	can_receive_msg->pt_data = &can_receive_buffer[0];			// Point to receive buffer
	can_receive_msg->ctrl.ide = 0;								// CAN 2.0A
	can_receive_msg->dlc	= BUF_LEN;								// Data buffer length
	can_receive_msg->id.std = STATE_ID;							// Set the receive ID to be the state ID
	can_receive_msg->cmd = CMD_RX_DATA_MASKED;					// Receive data
	
	// Enable RX
	while(can_cmd(can_receive_msg) != CAN_CMD_ACCEPTED);
}

/* init_UM6
	This will initialize the spi connection,
	send calibration commands to the um6,
	and set baud rate of GPS
	Arguments: void
	Returns: void
*/
void init_UM6(void) {
	SPI_init(64);
	
	/* Disable broadcast mode and set GPS baud rate to 115,200
	(bits 11-13: 101) */
// 	SPI_write(0x01); //0x01, write mode
// 	SPI_write(0x00); //0x00, communication register
// 	SPI_write(0x00); //0x00, no serial freq
// 	SPI_write(0b00101000); //0b00101000, GPS baud rate 115,200 
// 	SPI_write(0x00); //0x00, broadcast mode disabled
// 	SPI_write(0x00); //0x00, Not relevant
	
	/* Writes to UM6 configuration register 0x01:
	 enables gyro calibration on startup (bit 29: 1) */
// 	SPI_write(0x01); //0x01, write mode
// 	SPI_write(0x01); //0x01, configuration register
// 	SPI_write(0x00); //0x00, reserved
// 	SPI_write(0x00); //0x00, reserved
// 	SPI_write(0x00); //0x00, reserved
// 	SPI_write(0b00100000); //0b00100000, reserved
}