/*
 * dashboard_dio.c
 *
 * Created: 3/4/2015 8:42:27 PM
 *  Author: Robert
 */ 

#include <avr/io.h>
#include "standard_dio.h"
#include "spi.h"

uint8_t warning_LED_states = 0b00000000;
uint8_t button_illum_states = 0b00000000;

void set_DDRs() {
	/* data direction registers (input or output)
	 * these are specific to the dashboard PCB     */
	DDRB = 0b10111010;
	DDRC = 0b01001010;
	DDRD = 0b00011000;
	
	/* turn off all pull-ups */	
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
	
	/* turn on pull-ups where needed */
	sbi(PORTD,PORTD3); // SS pin - this is important! if this is left floating, chip randomly freezes in slave SPI mode. ick.
	sbi(PORTD,PORTD4); // SCK pin
}

uint8_t HVE_btn_pressed() {
	return is_high(PIND, PIND1);
}

uint8_t RTD_btn_pressed() {
	return is_high(PINC, PINC0);
}

uint8_t reset_btn_pressed() {
	return is_high(PINC, PINC5);
}

uint8_t DAQ_btn_pressed() {
	return is_high(PIND, PIND7);
}

uint8_t mode_btn_pressed() {
	return is_high(PINB, PINB0);
}

uint8_t left_disp_btn_pressed() {
	return is_high(PINC, PINC7);
}

uint8_t center_disp_btn_pressed() {
	return is_high(PINB, PINB6);
}

uint8_t right_disp_btn_pressed() {
	return is_high(PIND, PIND0);
}

void RTD_buzzer(uint8_t state) {
	if (state) {
		sbi(PORTC, PORTC1);
	} else {
		cbi(PORTC, PORTC1);
	}
}

void SPI_select_indicators(uint8_t state) {
	if (state) {
		cbi(PORTB, PORTB5);	 // left off
		cbi(PORTB, PORTB4);  // center off
		cbi(PORTB, PORTB3);  // right off
		sbi(PORTC, PORTC6);  // indicators on
		spi_send(0);
	} else {
		cbi(PORTC, PORTC6);  // indicators off
	}
}

// button_illum_states: 0b10000000 = HVE button, 0b01000000 = RTD button, 0b00100000 = reset button, 0b00010000 = DAQ button

void HVE_btn_illum(uint8_t state) {
	uint8_t prev_button_illum_states = button_illum_states;
	if (state) {
		sbi(button_illum_states, 7);
	} else {
		cbi(button_illum_states, 7);
	}
	if (prev_button_illum_states != button_illum_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void RTD_btn_illum(uint8_t state) {
	uint8_t prev_button_illum_states = button_illum_states;
	if (state) {
		sbi(button_illum_states, 6);
	} else {
		cbi(button_illum_states, 6);
	}
	if (prev_button_illum_states != button_illum_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void reset_btn_illum(uint8_t state) {
	uint8_t prev_button_illum_states = button_illum_states;
	if (state) {
		sbi(button_illum_states, 5);
	} else {
		cbi(button_illum_states, 5);
	}
	if (prev_button_illum_states != button_illum_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void DAQ_btn_illum(uint8_t state) {
	uint8_t prev_button_illum_states = button_illum_states;
	if (state) {
		sbi(button_illum_states, 4);
	} else {
		cbi(button_illum_states, 4);
	}
	if (prev_button_illum_states != button_illum_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

// warning_LED_states:  0b00001000 = top left (BSPD),   0b00000100 = top right (ERROR),  0b00000010 = bottom right (BMS), 0b00000001 = bottom left (IMD)

void BSPD_LED(uint8_t state) {
	uint8_t prev_warning_LED_states = warning_LED_states;
	if (state) {
		sbi(warning_LED_states, 3);
		} else {
		cbi(warning_LED_states, 3);
	}
	if (prev_warning_LED_states != warning_LED_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void IMD_LED(uint8_t state) {
	uint8_t prev_warning_LED_states = warning_LED_states;
	if (state) {
		sbi(warning_LED_states, 0);
		} else {
		cbi(warning_LED_states, 0);
	}
	if (prev_warning_LED_states != warning_LED_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void BMS_LED(uint8_t state) {
	uint8_t prev_warning_LED_states = warning_LED_states;
	if (state) {
		sbi(warning_LED_states, 1);
		} else {
		cbi(warning_LED_states, 1);
	}
	if (prev_warning_LED_states != warning_LED_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void ERROR_LED(uint8_t state) {
	uint8_t prev_warning_LED_states = warning_LED_states;
	if (state) {
		sbi(warning_LED_states, 2);
		} else {
		cbi(warning_LED_states, 2);
	}
	if (prev_warning_LED_states != warning_LED_states) {
		SPI_select_indicators(1);
		spi_send(button_illum_states);
		spi_send(warning_LED_states);
		SPI_select_indicators(0);
	}
}

void SPI_select_left_disp(uint8_t state) {
	if (state) {
		sbi(PORTB, PORTB5);	 // left on
		cbi(PORTB, PORTB4);  // center off
		cbi(PORTB, PORTB3);  // right off
		cbi(PORTC, PORTC6);  // indicators off
		spi_send(0);
	} else {
		cbi(PORTB, PORTB5);  // left off
	}
}

void SPI_select_center_disp(uint8_t state) {
	if (state) {
		cbi(PORTB, PORTB5);	 // left off
		sbi(PORTB, PORTB4);  // center on
		cbi(PORTB, PORTB3);  // right off
		cbi(PORTC, PORTC6);  // indicators off
		spi_send(0);
	} else {
		cbi(PORTB, PORTB4);  // center off
	}
}

void SPI_select_right_disp(uint8_t state) {
	if (state) {
		cbi(PORTB, PORTB5);	 // left off
		cbi(PORTB, PORTB4);  // center off
		sbi(PORTB, PORTB3);  // right on
		cbi(PORTC, PORTC6);  // indicators off
		spi_send(0);
	} else {
		cbi(PORTB, PORTB3);  // right off
	}
}

