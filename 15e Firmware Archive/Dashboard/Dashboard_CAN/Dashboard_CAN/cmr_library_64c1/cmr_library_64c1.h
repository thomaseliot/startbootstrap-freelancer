/*
 * cmr_library_64c1.h
 * 
 * Various helper functions for doing processor things
 * 
 * Created: 3/23/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATMega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 *	- None
 */ 

/*
 * cmr_library_64c1.h
 * Various helper functions for doing processor things
 *
 * Created: 3/23/2015 12:12:57 AM
 *  Author: Shepard Emerson
 */ 

#include <avr/io.h>
#include "can_lib.h"

//ADC Channel Macros, to read channels other than 1-10
#define TMP_SNS		11
#define VCC_DIV_4	12

//IO pin configuration macros, to choose ports and directions
#define IO_PORT_B		0
#define IO_PORT_C		1
#define IO_PORT_D		2
#define IO_DIR_INPUT	0
#define IO_DIR_OUTPUT	1

//IO pin configuration for SPI
#define IO_SPI_MISO	0	// PB0
#define IO_SPI_MOSI 1	// PB1
#define IO_SPI_SCK	7	// PB7
#define IO_SPI_SSA	1	// PC1

//IO read/write macros, to set i/o pins high or low
#define LOW		0
#define HIGH	1

// State Configuration, universal across boards
#define GLV_ON	0	// Grounded low-voltage on
#define HV_EN	1	// High voltage enable
#define RTD		2	// Ready to drive
#define ERROR	3	// Error

// Board heartbeat structs (barebones)
struct main_heartbeat_st {
	uint8_t state;
};

struct fsn_heartbeat_st {
	uint8_t state;
	uint8_t tps;
	uint8_t bps;
	uint8_t error;
};

struct rsn_heartbeat_st {
	uint8_t state;
};

struct um6_heartbeat_st {
	uint8_t state;
};

struct daq_heartbeat_st {
	uint8_t state;
};

struct dash_heartbeat_st {
	uint8_t state;
	unsigned int hv_en : 1;			// Bit fields, one bit each
	unsigned int clr_err : 1;
	unsigned int rtd : 1;
	unsigned int daq_en : 1;
};


//This is done in the programming menu, not in code.
//this is just a note
//void set_osc_src();

uint8_t adc_read(uint8_t ch);

/*

Takes: port, the i/o port to initialize (B,C,D)
		port_ch, the the specific port channel to configure (0...7)
		dir, the i/o direction, input or output 
*/
void config_io_pin(uint8_t port, uint8_t port_ch, uint8_t dir);

/*

Takes: port, the i/o port to initialize (B,C,D)
		port_ch, the the specific port channel to configure (0...7)
		val, the value to write, HIGH or LOW
*/
void set_io_pin(uint8_t port, uint8_t port_ch, uint8_t val);

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
void can_tx_sync(uint16_t ID, uint8_t *buffer, uint8_t buf_size);

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
uint8_t* can_rx_sync(uint16_t ID, uint8_t *buffer, uint8_t buf_size);

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
void SPI_init(uint8_t prescaler);

uint8_t SPI_transaction(uint8_t byte, uint8_t read_only);

uint8_t SPI_read(void);

void SPI_write(uint8_t byte);

//void SPI_write_buffer(uint8_t* buffer_ptr, uint8_t buffer_size);


/* setup_can_rcv
 * Initialize CAN mailbox to receive message msg_id 
 * Arguments:
 *  can_receive_msg: pointer to can cmd msg struct
 *  can_receive_buffer: pointer to buffer to receive msg_id into
 *  buffer_len: length of the buffer to receive
 */
void setup_can_rcv(st_cmd_t *can_receive_msg, uint8_t *can_receive_buffer, uint8_t buffer_len, uint16_t msg_id);


/* setup_can_send
 * Initialize CAN mailbox to send message msg_id 
 * Arguments:
 *  can_send_msg: pointer to can cmd msg struct
 *  can_send_buffer: pointer to buffer to send msg_id from
 *  buffer_len: length of the buffer to receive
 */
void setup_can_send(st_cmd_t *can_send_msg, uint8_t *can_send_buffer, uint8_t buffer_len, uint16_t msg_id);
