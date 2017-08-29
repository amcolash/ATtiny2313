/*
 * MCU: ATtiny2313
 * We use the display_image function to display a desired shape on the LED matrix. 
 
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

char x_shape[8] = {
	0b10000010,
	0b01000100,
	0b00101000,
	0b00010000,
	0b00101000,
	0b01000100,
	0b10000010,
	0b00000000  //notice inverse pattern
};

int main(void) {		
	DDRB = (1<<CLOCK) | (1<<CS) | (1<<DATA) ;

	matrix_data(0x0F,0x0);   //Display Test OFF
	matrix_data(0x0B,0x07);  //Scan Limit to digits 0 to 7.
	matrix_data(0x0C,0x1);   //Leave shutdown mode and enter normal operation

	display_image(x_shape);
	return 0;
}

