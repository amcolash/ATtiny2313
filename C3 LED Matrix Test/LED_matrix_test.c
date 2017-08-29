/*
 * MCU: ATtiny2313
 * We test the 8x8 LED matrix
 * Send 16 bits at a time: first 4 are Don't Cares. next 4 are ADDRESS. and then 8 DATA bits.
 * We send the instruction to enter "Display Test Mode", which turns on all LEDs at maximum brightness
 * Created by TronClub.com
 */

#include <avr/io.h>
#include <util/delay.h>

#define CLOCK PB0	//Rising edge of clock shift data one bit
#define CS	PB1		//the last 16 bits that were sent are latched on CS rising edge.
#define DATA PB2

int main(void) {		//set Display-test mode: Send 0000 1111 0000 0001
  DDRB = (1<<CLOCK) | (1<<CS) | (1<<DATA) ;
  display_test();
}

void display_test(void) { 	//0000 1111 0000 0001		All  LEDs on at maximum brightness
  send_zero();
  send_zero();
  send_zero();
  send_zero();

  send_one();
  send_one();
  send_one();
  send_one();

  send_zero();
  send_zero();
  send_zero();
  send_zero();

  send_zero();
  send_zero();
  send_zero();
  send_one();

  matrix_read();
}

void send_zero(void) {  // clear DATA and CLOCK, then set CLOCK.
  PORTB &= ~(1<<DATA | 1<<CLOCK);
  PORTB |= (1<<CLOCK);
}

void send_one(void) {		//set DATA and clear CLOCK, then set CLOCK.
  PORTB = (1<<DATA);
  PORTB |= (1<<CLOCK);		//sets DATA = 1 		then sets CLOCK.
}

void matrix_read(void) {  		//Set CS as HIGH, then LOW to read the last 16 bits
  PORTB |= (1<<CS);
  _delay_ms(2);
  PORTB &= ~(1<<CS);
  _delay_ms(2);
}
