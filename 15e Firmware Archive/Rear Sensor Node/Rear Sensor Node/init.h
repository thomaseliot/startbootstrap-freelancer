/*
 * init.h
 * 
 * The header file for standard CMR initialization functions
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
 *	- None
 */

#include <avr/io.h>
#include <avr/wdt.h>
#include "can_lib.h"
#include "config.h"
#include "can_ids.h"


//Declare CAN mailboxes
extern st_cmd_t heartbeat_cmd_st;	// One send command for heartbeat send mailbox
extern st_cmd_t rcv_cmd_st[];	// Five receive commands for remaining five mailboxes

//Declare buffer to receive into
extern uint64_t can_rcv_buffer[];
extern uint64_t can_send_buffer;


/* Declare struct to send RSN heartbeat messages */
extern struct rsn_heartbeat_st rsn_heartbeat;

// Initialization Functions

/*
 * Initialize interrupts
 */
void init_interrupts(void);

/*
 * Initialize CAN
 */
void init_can(void);

/*
 * Initialize ADC
 */
void init_adc(void);

/*
 * Initialize IO pins
 */
void init_io(void);



/* init_can_mailboxes
 * sets up mailboxes for all critical CAN messages that this board receives
 *
 */

void init_can_mailboxes(void);