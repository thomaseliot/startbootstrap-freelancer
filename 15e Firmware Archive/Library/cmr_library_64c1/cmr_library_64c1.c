/*
 * cmr_library_64c1.c
 * Various helper functions for doing processor things
 * 
 * Created: 3/23/2015
 * Author: Shepard Emerson (semerson), Daniel Haddox (dhaddox), Daniel Gorziglia (dgrozigl)
 * Platform: ATMega64C1
 * Modified: 4/22/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 *	- None
 */

#include <avr/io.h>
#include "cmr_library_64c1.h"

/************************************************************************/
/* MCU Initialization                                                   */
/************************************************************************/

/* adc_read
 * Read an ADC channel, blocking until conversion is complete.
 * Arguments:
 *	ch: the channel number to read
 * Returns: the result of the conversion, 8-bit
 */
uint8_t adc_read(uint8_t ch) {
	// Set which channel to read. Leave all ADMUX bits as is except MUX4-0
	
	// Clear MUX bits
	ADMUX &= 0xE0;
	// Guard against invalid channel
	if(ch > 12) ch = 18; // Read ground
	// Set MUX to channel
	ADMUX |= ch;
	
	// Set ADC start conversion bit
	ADCSRA |= (1 << ADSC);
	
	// Wait for ADCSRA bit ADSC to go low for data conversion to complete
	while(ADCSRA & (1 << ADSC));
	
	// Return read channel
	return ADCH;
}

/* config_io_pin
 * Configures an digital I/O pin as input or output.
 * Arguments:
 *	port: the port to configure, IO_PORT_B, C, or D
 *  port_ch: the channel to set, 0-7
 *  dir: the direction, IO_DIR_INPUT or IO_DIR_OUTPUT
 * Returns: void
 */
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

/* set_io_pin
 * Sets an digital I/O pin as high or low.
 * Arguments:
 *	port: the port to configure, IO_PORT_B, C, or D
 *  port_ch: the channel to set, 0-7
 *  val: the value, HIGH or LOW
 * Returns: void
 */
void set_io_pin(uint8_t port, uint8_t port_ch, uint8_t val){
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

/************************************************************************/
/* CAN Library Helpers                                                  */
/************************************************************************/

/* can_tx_sync
 * 
 * Transmit a message over CAN and wait for transmit to complete.
 * 
 * Arguments: 
 *		ID, the CAN ID to send with
 *		buffer, an array of things to send
 *		buf_size, the size of the buffer
 * 
 * Returns:
 *		void	
 */
void can_tx_sync(uint16_t id, uint8_t *buffer, uint8_t buf_size) {
	// Message command object
	st_cmd_t can_msg;
	
	// Configure TX mailbox
	can_msg.pt_data = buffer;	// point message object to the data buffer
	can_msg.ctrl.ide = 0;		// CAN 2.0A
	can_msg.dlc = buf_size;		// Number of data bytes (8 max) requested from remote node
	can_msg.id.std = id;		// Message ID
	can_msg.cmd = CMD_TX_DATA;	// Transmit data command object

	// Wait for MOb to configure (Must re-configure MOb for every transaction) and send request
	while(can_cmd(&can_msg) != CAN_CMD_ACCEPTED);
	// Wait for Tx to complete
	while(can_get_status(&can_msg) == CAN_STATUS_NOT_COMPLETED);
}


/* can_rx_sync
 * 
 * Receive a message over CAN and wait for receive to complete.
 * 
 * Arguments:
 *		ID, the CAN ID to send with
 *		buffer, an array of things to send
 *		buf_size, the size of the buffer
 *
 * Returns:
 *		the received array
 */
void can_rx_sync(uint16_t ID, uint8_t *buffer, uint8_t buf_size) {
	// Message command object
	st_cmd_t can_msg;
	
	// Configure TX mailbox
	can_msg.pt_data = buffer;			// point message object to the data buffer
	can_msg.ctrl.ide = 0;				// CAN 2.0A
	can_msg.dlc = buf_size;				// Number of data bytes (8 max) requested from remote node
	can_msg.id.std = ID;				// Message ID
	can_msg.cmd = CMD_RX_DATA_MASKED;	// Receive data command object

	// Wait for MOb to configure (Must re-configure MOb for every transaction) and send request
	while(can_cmd(&can_msg) != CAN_CMD_ACCEPTED);
	// Wait for Tx to complete
	while(can_get_status(&can_msg) == CAN_STATUS_NOT_COMPLETED);
}

/************************************************************************/
/* SPI Helpers                                                          */
/************************************************************************/

/* SPI_init
 * 
 * Initializes SPI on the MCU
 * 
 * Arguments: 
 *		prescaler -	prescaler value to set SCK (SPI Clock). Bus clock / prescaler = SCK.  
 *					Values are 2, 4, 6, 8, 16, 32, 64, 128
 * 
 * Returns:
 *		void	
 */
void SPI_init(uint8_t prescaler) {
	// Set digital I/O pins
	config_io_pin(IO_PORT_B, IO_SPI_MISO, IO_DIR_INPUT);
	config_io_pin(IO_PORT_B, IO_SPI_MOSI, IO_DIR_OUTPUT);
	config_io_pin(IO_PORT_B, IO_SPI_SCK, IO_DIR_OUTPUT);
	config_io_pin(IO_PORT_C, IO_SPI_SSA, IO_DIR_OUTPUT);
	
	set_io_pin(IO_PORT_C, IO_SPI_SSA, HIGH);
	
	// Enable SPI, and Master
	SPCR = (1 << SPE) | (1 << MSTR); 
	
	// Set the appropriate SPI SCK in the SPI Control Register
	// Set based off of table 15-4 in 64C1 data sheet. 
	// Control is [SPI2X SPR1 SPR0]
	switch (prescaler) {
		case 2:
			// 1 0 0
			SPSR |= (1 << SPI2X);  
			break;
		case 4:
			// 0 0 0
			break;
		case 8:
			// 1 0 1
			SPSR |= (1 << SPI2X);
			SPCR |= (1 << SPR0); 
			break;
		case 16:
			// 0 0 1
			SPCR |= (1 << SPR0); 
			break;
		case 32:
			// 1 1 0
			SPSR |= (1 << SPI2X);
			SPCR |= (1 << SPR1);
			break;
		case 128:
			// 0 1 1
			SPCR |= (1 << SPR1) | (1 << SPR0);
			break;
		case 64:
		default:
			// 1 1 1
			SPSR |= (1 << SPI2X);
			SPCR |= (1 << SPR1) | (1 << SPR0);
			break;
	}
}

/* SPI_transaction
 * 
 * Writes a byte over SPI and receive a byte
 * 
 * Arguments:
 *		byte - byte to send over SPI
 *		read_only - if set to 1, the data register will not be touched before the transaction begins. 
 * 
 * Returns:
 *		received byte
 */
uint8_t SPI_transaction(uint8_t byte, uint8_t read_only) {
	// Set data register
	if (!read_only) {
		SPDR = byte;	
	}
	
	// Wait for transfer to complete
	while (!(SPSR & (1 << SPIF)));
	
	// Return received value
	return SPDR;
}

/* SPI_read
 * 
 * Reads one byte from SPI. Does not touch the SPI data register before transaction. 
 * 
 * Arguments:
 *		void
 * 
 * Returns:
 *		received byte
 */
uint8_t SPI_read(void) {
	return SPI_transaction(0, 1);
}

/* SPI_write
 * 
 * Writes a byte over SPI and ignore return value
 * 
 * Arguments:
 *		byte - byte to send over SPI
 * 
 * Returns:
 *		void
 */
void SPI_write(uint8_t byte) {
	
	(void)SPI_transaction(byte, 0);
}




/* setup_can_rcv
 * Initialize CAN mailbox to receive message msg_id 
 * Arguments:
 *  can_receive_msg: pointer to can cmd msg struct
 *  can_receive_buffer: pointer to buffer to receive msg_id into
 *  buffer_len: length of the buffer to receive
 */
void setup_can_rcv(st_cmd_t *can_receive_msg, uint8_t *can_receive_buffer, uint8_t buffer_len, uint16_t msg_id) {
	// Populate message object
	can_receive_msg->pt_data = &can_receive_buffer[0];			// Point to receive buffer
	can_receive_msg->ctrl.ide = 0;								// CAN 2.0A
	can_receive_msg->dlc	= buffer_len;							// Data buffer length
	can_receive_msg->id.std = msg_id;							// Set the receive ID to be the state ID
	can_receive_msg->cmd = CMD_RX_DATA_MASKED;					// Receive data
	
	/* Wait for an open mailbox and bind msg_id to the mailbox when available*/
	
	while(can_cmd(can_receive_msg) != CAN_CMD_ACCEPTED);
}

/* setup_can_send
 * Initialize CAN mailbox to send message msg_id 
 * Arguments:
 *  can_send_msg: pointer to can cmd msg struct
 *  can_send_buffer: pointer to buffer to send msg_id from
 *  buffer_len: length of the buffer to send
 */
void setup_can_send(st_cmd_t *can_send_msg, uint8_t *can_send_buffer, uint8_t buffer_len, uint16_t msg_id) {
	// Populate message object
	can_send_msg->pt_data = &can_send_buffer[0];				// Point to send buffer
	can_send_msg->ctrl.ide = 0;									// CAN 2.0A
	can_send_msg->dlc	= buffer_len;							// Data buffer length
	can_send_msg->id.std = msg_id;								// Set the receive ID to be the state ID
	can_send_msg->cmd = CMD_TX_DATA;							// Send data
	
	/* Wait for an open mailbox and bind msg_id to the mailbox when available*/
	while(can_cmd(can_send_msg) != CAN_CMD_ACCEPTED);
}