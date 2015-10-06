/*
 * display.h
 *
 * Created: 3/4/2015 11:48:10 PM
 *  Author: Robert
 */ 


#ifndef DISPLAY_H_
#define DISPLAY_H_


char getDigitBitPattern(uint8_t digit);
char getCharBitPattern(char character);

char thirdOut(char digit, char rgb);
char secondOut(char digit, char rgb);
char firstOut(char digit, char rgb);

void writeDigit(char dig, char rgb);
void writeCharacter(char character, char rgb);

void write_value(short val, char rgb);

void display_int(char which_display, short val, char rgb, char LEDs);
void display_str(char which_display, char string[3], char rgb, char LEDs);


#endif /* DISPLAY_H_ */