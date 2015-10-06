/*
 * CMR_Dashboard.c
 *
 * Created: 3/18/2015 7:12:39 PM
 *  Author: Robert
 *
 *  To Do:
 *    Why do all displays receive things even when their SS pin is not selected?
 *    Why does the processor feeze when the programmer is not plugged in?
 *    
 *
 *  SUT_CKSEL Fuse Setting: EXTOSC_8MHZ_XX_16KCK_14CK_65MS
 */ 

#define F_CPU 2000000UL

#include <avr/io.h>
#include <util/delay.h>
#include "dashboard_dio.h"
#include "standard_dio.h"
#include "spi.h"
#include "display.h"

int main(void)
{
	set_DDRs();
	spi_init(0, 1, 0, 3, 0);
	
	while(1) {
		display_str(1,"Err",0b100,0b1000);
		_delay_ms(1000);
		display_str(1,"ACC",0b010,0b1000);
		_delay_ms(1000);
		display_str(1,"End",0b010,0b1000);
		_delay_ms(1000);
		display_str(1,"rtd",0b111,0b1000);
		_delay_ms(1000);
		display_str(1,"hot",0b100,0b1000);
		_delay_ms(1000);
		display_str(1,"col",0b001,0b1000);
		_delay_ms(1000);
		display_str(1,"bob",0b110,0b1000);
		_delay_ms(1000);
		display_str(1,"lol",0b101,0b1000);
		_delay_ms(1000);
		display_str(1,"HOE",0b011,0b1000);
		_delay_ms(1000);
			
	}
}