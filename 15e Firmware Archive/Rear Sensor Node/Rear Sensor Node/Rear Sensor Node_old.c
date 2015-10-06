/*
 * File Rear_Sensor_Node.c
 * Description: The main fail for the rear sensor node
 * Created: 3/22/2015 8:03:13 PM
 * Author: Daniel Haddox, dhaddox
 */ 


#include <avr/io.h>
#include "init.h"
#include "cmr_library_64c1.h"

/* Function: example_function
 * Description: this is a brief description of the functions use
 * Takes: example_arg1, the first example argument; example_arg2,
 *		  the second example argument 
 * Returns: example_ret_val,the example return value
 * Errors: none
 * Author: Daniel Haddox, dhaddox
 * Last Edit: 3/22/15
 */

int main(void)
{
	//Initialize MCU parameters
    //init_mcu();
	//Initialize all board  
	init_io();
	init_adc();
	init_interrupts();
	while(1)
    {
        //TODO:: Please write your application code 
    }
}


//Main 100Hz interrupt handler
ISR(TIMER1_CMPA_vect){
	
	
}