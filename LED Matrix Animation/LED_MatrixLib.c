/*
 * MCU: ATtiny2313
 * A library to control the 8x8 LED matrix.
 * The send_data function takes in 8 bits, and for each bit, sets the DATA pin to 0 to 1, and then triggers the CLOCK pin, to send one bit of data.
 * The matrix_data function calls the send_data function twice to send 16 bits and triggers the CS pin so the LED matrix reads the 16 bits. 
 * the display_image function takes an array of 8 elements, where each element represents the state of the LEDs in one column. 
 * Created by TronClub.com
 */

#include <avr/io.h>
#include <util/delay.h> 

#define CLOCK PB0	//Rising edge of clock shift data one bit
#define CS	PB1		//the last 16 bits that were sent are latched on CS rising edge.
#define DATA PB2

void send_data(unsigned char data) {
  unsigned char i;

  for (i = 0; i < 8; i++) {
	  PORTB &= ~(1<<CLOCK);
	  
	  if (data & 0x80) {   //bit compare with 10000000
      PORTB |= (1<<DATA);
	  } else {
      PORTB &= ~(1<<DATA);
    }
	      
	  PORTB |= (1<<CLOCK);
	  data <<= 1;
  }
}

void matrix_data(unsigned char register_byte, unsigned char data_byte) {
	PORTB &= ~(1<<CS);
	send_data(register_byte);
	send_data(data_byte);
	PORTB |= (1<<CS);
}

void display_image (unsigned char image[]) {
	unsigned char i;
	for (i=0;i<8;i++) {
		matrix_data(i+1,image[i]);		//first increment from 0001 to 1000, so they control the 8 columns.
	}								                //the image[i] byte controls which LEDs are on in each column. 
}

