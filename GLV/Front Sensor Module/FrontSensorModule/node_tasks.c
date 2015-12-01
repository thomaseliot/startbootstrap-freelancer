/*
 * tasks.c
 *
 * Created: 11/23/2015 6:19:47 PM
 *  Author: skir0
 */ 

#include "node_tasks.h"
#include "task.h"
#include "can_ids.h"
#include <avr/io.h>

st_cmd_t heartbeat_cmd_st; //Receive command for 1 reserved heartbeat mailbox
uint64_t can_send_buffer;


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
void can_tx(uint16_t id, uint8_t *buffer, uint8_t buf_size) {
	
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

void vLEDFlashTask(void *pvParameters) {
	for(;;) {
		PORTC ^= (1 << 0);
		vTaskDelay((TickType_t)500); 
	}
}

void vHeartbeatTask(void *pvParameters) {
	uint8_t send_data = 0x01;
	
	for(;;) {
		can_tx(FSN_HEARTBEAT_ID, &send_data, 1);
		vTaskDelay((TickType_t)1); 
	}
}