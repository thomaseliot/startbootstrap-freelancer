/*
 * can_callbacks.c
 * 
 * Callback functions for CAN receive
 *
 * Copyright (c) Carnegie Mellon Racing 2016
 */ 


#include "can_callbacks.h"

// Create these based on CAN configuration. They are bound to 
// receive mailboxes manually in the main function.

// Mailbox 1 callback function
void mb1_callback(CAN_packet packet) {
	volatile int i, j;
	i = j;
}


// Mailbox 2 callback function
void mb2_callback(CAN_packet packet) {
	volatile int i, j;
	j = i;
}


// Mailbox 3 callback function
void mb3_callback(CAN_packet packet) {
	volatile int i, j;
	i = j;
}


// Mailbox 4 callback function
void mb4_callback(CAN_packet packet) {
	volatile int i, j;
	i = j;
}