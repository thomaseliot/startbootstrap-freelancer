/*
 * can_callbacks.c
 *
 * Created: 2/25/2016 10:00:15 PM
 *  Author: semerson
 */ 

#include "can_callbacks.h"

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
