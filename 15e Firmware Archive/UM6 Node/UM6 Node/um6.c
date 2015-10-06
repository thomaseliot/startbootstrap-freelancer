/*
 * um6.c
 * UM6 helper functions 
 * 
 * Created: 4/23/2015
 * Author:	Daniel Gorziglia	(dgorzigl)
			Suyash Bhatt		(sbhatt)
 *			Shepard Emerson		(semerson)
 * Platform: ATmega64C1
 * Modified: 4/23/2015
 * 
 */

#include "um6.h"
#include "cmr_library_64c1.h"

/* um6_command
 * Perform a transaction (send and receive) to read/write/command registers
 * in the UM6
 * Arguments:
 *	cmd_type: the command type. UM6_READ, UM6_WRITE, or UM6_CMD
 *  reg: the register ID to use (use macros)
 *  buffer_ptr: a pointer to the buffer to send
 *  buffer_len: the length of the buffer
 * Returns: none
 */
void um6_command(uint8_t cmd_type, uint8_t reg, uint8_t* buffer_ptr, uint8_t buffer_len) {
	// Loop counter
	uint8_t i;
	
	// Set SS active LOW to initiate transaction
	set_io_pin(IO_PORT_C, IO_SPI_SSA, LOW);
	
	// Case on the command type
	switch (cmd_type) {
		case UM6_READ:
			// Reads on the UM6 are done by sending a READ opcode followed by the data register of interest.
			// We then perform a SPI transaction for each byte that we want to collect by sending 0x00
			// to keep the MOSI line active.
			SPI_write(0x00); // Send a read operation
			SPI_write(reg); // Send register of interest
			
			// Read the results from the UM6
			for (i = 0; i < buffer_len; i++) {
				buffer_ptr[i] = SPI_transaction(0x00, 0);
			}
			break;
			
		case UM6_WRITE:
			// A write operation is performed by sending a WRITE opcode followed by the register of interest.
			// We then write our buffer over SPI to the UM6.
			SPI_write(0x01); // Send a write operation
			SPI_write(reg); // Send register of interest
		
			// Send buffer
			for (i = 0; i < buffer_len; i++) {
				SPI_write(buffer_ptr[i]);
			}
			break;
			
		case UM6_CMD:
			// Commands on the UM6 are done by sending the opcode followed by 4 bytes of 0x00.
			// The UM6 will not report that a command was executed over the SPI bus except
			// for during a GET_FW_REVISION command. This function still fills the buffer
			// with what is in the SPI data register but note that it might be useless data
			// unless you care about the UM6 firmware revision.
			SPI_write(0x01); // Send a command operation
			SPI_write(reg); // Send command opcode of interest
			// Send 0x00's for command to run
			for (i = 0; i < UM6_CMD_LEN; i++) {
				buffer_ptr[i] = SPI_transaction(0x00, 0);
			}
			break;
			
		default:
			break;
	}
	
	// Set SS to HIGH to denote end of operation
	set_io_pin(IO_PORT_C, IO_SPI_SSA, HIGH);
}