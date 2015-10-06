/*
 * display.c
 *
 * Created: 3/4/2015 11:00:39 PM
 *  Author: Robert
 */ 


#include "spi.h"
#include "dashboard_dio.h"

/* Input a digit (0 - 9),
 * outputs a bit pattern in the form of (dp)abcdef for the 7-seg display   */
char getDigitBitPattern(uint8_t digit) {
  switch (digit) {
    case 0: return 0b01111110;
    case 1: return 0b00110000;
    case 2: return 0b01101101;
    case 3: return 0b01111001;
    case 4: return 0b00110011;
    case 5: return 0b01011011;
    case 6: return 0b01011111;
    case 7: return 0b01110000;
    case 8: return 0b01111111;
    case 9: return 0b01111011;
    default: return 0b00000000;
  }
}

/* Input a character (0 - 9),
 * outputs a bit pattern in the form of (dp)abcdef for the 7-seg display   */
char getCharBitPattern(char character) {
  switch (character) {
    case 'A': return 0b01110111;
    case 'B': return 0b01111111;
	case 'C': return 0b01001110;
	case 'D': return 0b01111110;
	case 'E': return 0b01001111;
	case 'F': return 0b01000111;
	case 'G': return 0b01011111;
	case 'H': return 0b00110111;
	case 'I': return 0b00000110;
	case 'J': return 0b00111100;
	case 'K': return 0b00110111;
	case 'L': return 0b00001110;
	//case 'M': return 0b0;
	case 'N': return 0b00010101;
	case 'O': return 0b01111110;
	case 'P': return 0b01100111;
	case 'Q': return 0b01110011;
	case 'R': return 0b00000101;
	case 'S': return 0b01011011;
	case 'T': return 0b00001111;
	case 'U': return 0b00111110;
	case 'V': return 0b00111110;
	//case 'W': return 0b0;
	case 'X': return 0b00110111;
	case 'Y': return 0b00111011;
	case 'Z': return 0b01101101;
	case 'a': return 0b01110111;
	case 'b': return 0b00011111;
	case 'c': return 0b00001101;
	case 'd': return 0b00111101;
	case 'e': return 0b01001111;
	case 'f': return 0b01000111;
	case 'g': return 0b01111011;
	case 'h': return 0b00010111;
	case 'i': return 0b00000100;
	case 'j': return 0b00111000;
	case 'k': return 0b00110111;
	case 'l': return 0b00000110;
	//case 'm': return 0b0;
	case 'n': return 0b00010101;
	case 'o': return 0b00011101;
	case 'p': return 0b01100111;
	case 'q': return 0b01110011;
	case 'r': return 0b00000101;
	case 's': return 0b01011011;
	case 't': return 0b00001111;
	case 'u': return 0b00011100;
	case 'v': return 0b00011100;
	//case 'w': return 0b0;
	case 'x': return 0b00110111;
	case 'y': return 0b00111011;
	case 'z': return 0b01101101;
	case ' ': return 0b00000000;
	case '_': return 0b00001000;
	case '-': return 0b00001000;
	case '0': return getDigitBitPattern(0);
	case '1': return getDigitBitPattern(1);
	case '2': return getDigitBitPattern(2);
	case '3': return getDigitBitPattern(3);
	case '4': return getDigitBitPattern(4);
	case '5': return getDigitBitPattern(5);
	case '6': return getDigitBitPattern(6);
	case '7': return getDigitBitPattern(7);
	case '8': return getDigitBitPattern(8);
	case '9': return getDigitBitPattern(9);
    default: return 0b00000000;
  }
}
// calculates byte to send to the third shift register for this character (closest to SPI source)
char thirdOut(char digit, char rgb) {
  char final = 0b0;
  //         (dp)abcdefg             rgb
  if ((digit & 0b00000100) && (rgb & 0b010)) final |= 0b1;
  if ((digit & 0b00001000) && (rgb & 0b001)) final |= 0b10;
  if ((digit & 0b00001000) && (rgb & 0b010)) final |= 0b100;
  if ((digit & 0b00001000) && (rgb & 0b100)) final |= 0b1000;
  if ((digit & 0b00010000) && (rgb & 0b001)) final |= 0b10000;
  if ((digit & 0b00010000) && (rgb & 0b100)) final |= 0b100000;
  if ((digit & 0b10000000) && (rgb & 0b010)) final |= 0b1000000;
  if ((digit & 0b10000000) && (rgb & 0b100)) final |= 0b10000000;
  return final;
}

// calculates byte to send to the second shift register for this character 
char secondOut(char digit, char rgb) {
  char final = 0b0;
  //         (dp)abcdefg             rgb
  if ((digit & 0b00000100) && (rgb & 0b001)) final |= 0b1;
  if ((digit & 0b00000100) && (rgb & 0b100)) final |= 0b10;
  if ((digit & 0b00010000) && (rgb & 0b010)) final |= 0b100;
  if ((digit & 0b10000000) && (rgb & 0b001)) final |= 0b1000;
  if ((digit & 0b00000010) && (rgb & 0b010)) final |= 0b10000;
  if ((digit & 0b00000010) && (rgb & 0b100)) final |= 0b100000;
  if ((digit & 0b00000001) && (rgb & 0b010)) final |= 0b1000000;
  if ((digit & 0b00100000) && (rgb & 0b010)) final |= 0b10000000;
  return final;
}

// calculates byte to send to the first shift register for this character (furthest from SPI source)
char firstOut(char digit, char rgb) {
  char final = 0b0;
  //         (dp)abcdefg             rgb
  if ((digit & 0b00000010) && (rgb & 0b001)) final |= 0b1;
  if ((digit & 0b00000001) && (rgb & 0b001)) final |= 0b10;
  if ((digit & 0b00000001) && (rgb & 0b100)) final |= 0b100;
  if ((digit & 0b01000000) && (rgb & 0b001)) final |= 0b1000;
  if ((digit & 0b01000000) && (rgb & 0b010)) final |= 0b10000;
  if ((digit & 0b01000000) && (rgb & 0b100)) final |= 0b100000;
  if ((digit & 0b00100000) && (rgb & 0b001)) final |= 0b1000000;
  if ((digit & 0b00100000) && (rgb & 0b100)) final |= 0b10000000;
  return final;
}

// writes a digit with the specified color (rbg is 3-bit binary set of flags). 
void writeDigit(char dig, char rgb) {
  char digit = getDigitBitPattern(dig);
  char first = firstOut(digit, rgb);
  char second = secondOut(digit, rgb);
  char third = thirdOut(digit, rgb);
  //shift first
  spi_send(first);
  //shift second
  spi_send(second);
  //shift third
  spi_send(third);
}

// writes a character with the specified color (rbg is 3-bit binary set of flags).
void writeCharacter(char character, char rgb) {
	char ch = getCharBitPattern(character);
	char first = firstOut(ch, rgb);
	char second = secondOut(ch, rgb);
	char third = thirdOut(ch, rgb);
	//shift first
	spi_send(first);
	//shift second
	spi_send(second);
	//shift third
	spi_send(third);
}

// writes a value to a display (a three-digit decimal number)
void write_value(short val, char rgb) {
	if (val > 999)
		val = 999;
	if (val < 0)
		val = -val;
	
	char hundreds_digit = val / 100;
	char tens_digit = (val / 10) % 10;
	char ones_digit = val  % 10;
	
	char rgb_hundreds = rgb;
	char rgb_tens = rgb;
	
	if (hundreds_digit == 0)
		rgb_hundreds = 0b000;
	
	if (hundreds_digit == 0 && tens_digit == 0)
		rgb_tens = 0b000;
	
	// hundreds place character
	writeDigit(hundreds_digit, rgb_hundreds);
	
	// tens place character
	writeDigit(tens_digit, rgb_tens);
	
	// ones place character
	writeDigit(ones_digit, rgb);

}

/* Performs all actions needed to update a display module.
 * which_display:  1 = left, 2 = center, 3 = right
 * val:			   a numerical value to display (0 to 999)
 * rgb:			   what color to use (eg. 0b100 is red, 0b101 is purple)
 * LEDs:		   which indicator LEDs to turn on    
 *
 * Usage: display_int(1,123,0b001,0b0010);
 */
void display_int(char which_display, short val, char rgb, char LEDs) {
	switch(which_display) {
		case 1:
			SPI_select_left_disp(1);
			break;
		case 2:
			SPI_select_center_disp(1);
			break;
		case 3:
			SPI_select_right_disp(1);
			break;
	}
	spi_send(LEDs);
	write_value(val, rgb);
	switch(which_display) {
		case 1:
			SPI_select_left_disp(0);
			break;
		case 2:
			SPI_select_center_disp(0);
			break;
		case 3:
			SPI_select_right_disp(0);
			break;
	}
}

/* Display three characters on a display.
 * which_display:  1 = left, 2 = center, 3 = right
 * string:		   3 characters to display. Upper and lower case characters accepted, except w, W, m, M.
 *											Digits 0-9, space, dash, and underscore are also accepted. 
 * rgb:			   what color to use (eg. 0b100 is red, 0b101 is purple)
 * LEDs:		   which indicator LEDs to turn on  (eg. 0b1010 turns on the 1st and 4th leds, (top is 1st)
 *
 * Usage: display_str(1,"hot",0b100,0b1000);
 */

void display_str(char which_display, char string[3], char rgb, char LEDs) {
	switch(which_display) {
		case 1:
			SPI_select_left_disp(1);
			break;
		case 2:
			SPI_select_center_disp(1);
			break;
		case 3:
			SPI_select_right_disp(1);
			break;
	}
	spi_send(LEDs);
	if (string[0] == '\n') {
		writeCharacter(' ', rgb);	
		writeCharacter(' ', rgb);	
		writeCharacter(' ', rgb);	
	} else {
		writeCharacter(string[0], rgb);	
		if (string[1] == '\n') {
			writeCharacter(' ', rgb);
			writeCharacter(' ', rgb);
		} else {
			writeCharacter(string[1], rgb);
			if (string[2] == '\n') {
				writeCharacter(' ', rgb);
			} else {
				writeCharacter(string[2], rgb);
			}
		}
	}
	
	switch(which_display) {
		case 1:
			SPI_select_left_disp(0);
			break;
		case 2:
			SPI_select_center_disp(0);
			break;
		case 3:
			SPI_select_right_disp(0);
			break;
	}
}