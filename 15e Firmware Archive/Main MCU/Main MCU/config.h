/*
 * config.h
 * Board-specific configuration
 * 
 * Created: 4/10/2015
 * Author: Shepard Emerson (semerson)
 * Platform: ATmega64C1
 * Modified: 4/15/2015
 * 
 * Change List:
 *  - 4/15/15: Added change list and known errors to file header (Shepard Emerson)
 * 
 * Known Errors:
 *	- CAN frequency is locked at 16MHz. Need to restructure CAN library to fix (Shepard Emerson)
 */

// Check if this has been defined yet, if not, define everything
#ifndef _CONFIG_H_
#define _CONFIG_H_

// DEFINITIONS
// Digital Port Configuration
#define MCU_STATUS_PORT			IO_PORT_D	// MCU Status is port D pin 7
#define MCU_STATUS_PIN			7	

// Watchdog timeout
#define WATCHDOG_TIMEOUT		WDTO_120MS	// 120ms timeout

#endif  // _CONFIG_H_