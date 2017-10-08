/*
 * MCU: ATtiny26L
 * We animate the LED matrix by switching from one shape to the next with delays.
 
 * Send 16 bits at a time: first 4 are Don't Cares. next 4 are ADDRESS. and then 8 DATA bits.
 * The 4 ADDRESS bits allow us to choose a column, and then the 8 DATA bits represent which of the 8 LEDs in that column are on/off 
 * However, before we can control the LEDs, we must initialize the LED Matrix:
 * Upon powering up, we're in shutdown mode and only scan 1 digit (i.e only the leftmost column can be controlled).
 * To initialize: Leave shutdown mode (i.e. enter Normal Operation mode), then set Scan Limit to digits 0 to 7. This way, we can control all the columns.
 * Created by TronClub.com
 */

#include <avr/io.h>
#include <util/delay.h> 

#define CLOCK PB0	//Rising edge of clock shift data one bit
#define CS	PB1		//the last 16 bits that were sent are latched on CS rising edge.
#define DATA PB2

// the pattern is rotated 90 degree clockwise when inserted directly into breadboard
char smiley_shape[8] = {
	0b00111100,
	0b01000010,
	0b10010101,
	0b10100001,
	0b10100001,
	0b10010101,
	0b01000010,
	0b00111100
};

char scream_shape[8] = {
	0b00111100,
	0b01000010,
	0b10000101,
	0b10110001,
	0b10110001,
	0b10000101,
	0b01000010,
	0b00111100
};

char frown_shape[8] = {
	0b00111100,
	0b01000010,
	0b10100101,
	0b10010001,
	0b10010001,
	0b10100101,
	0b01000010,
	0b00111100
};

//char smiley_shape[8] = {0x7e,0x83,0xa9,0x81,0xc5,0xb9,0x82,0x7c}; //HEX bitmap
//char frown_shape[8] = {0x7e,0x83,0xa9,0x81,0xb9,0xa9,0x82,0x7c}; //HEX bitmap

int main(void) {		
	DDRB = (1<<CLOCK) | (1<<CS) | (1<<DATA);

	matrix_data(0x0F,0x0);		//Display Test OFF
	matrix_data(0x0B,0x07);		//Scan Limit to digits 0 to 7.
	matrix_data(0x0C,1); 		  //Leave shutdown mode and enter normal operation
	matrix_data(0x0A,0x1);		//Second argument is a number from 0 to 16 (i.e 0x0 to 0xF), to choose intensity.

	while(1) {
		display_image(smiley_shape);
		_delay_ms(1000);
		display_image(scream_shape);
		_delay_ms(1000);
		display_image(frown_shape);
		_delay_ms(1000);
	}
	
	return 0;
}

