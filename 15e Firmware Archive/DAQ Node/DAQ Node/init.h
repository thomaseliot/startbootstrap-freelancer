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
 * Initialize IO pins
 */
void init_io(void);


/* Initialize state receive message
 * 
 */
void init_state_capture(st_cmd_t *can_receive_msg, uint8_t *can_recieve_buffer, uint8_t BUF_LEN);