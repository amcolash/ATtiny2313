/*
 * MCU: ATtiny2313
 * We use the timer compare interrupt: timer0 counts up and compares its value to OCR0A.
 * When the timer value equals OCR0A, the ISR is executed.
 * We gradually speed up the flashing of an LED by decrementing OCR0A every time the interrupt is triggered.  
 * Created by TronClub.com
 */

#include <avr/io.h>
#include <util/delay.h> 
#include <avr/interrupt.h>

#define LED PD5

int main(void) {
	DDRD = (1<<PD5);
	sei();
	TCCR0B = (1<<CS02)|(1<<CS00); 		//prescaler /256
	OCR0A = 255;
	TIMSK = (1<<OCIE0A);
	
	while(1) { }
	
	return 0;
}

ISR(TIMER0_COMPA_vect) {
	TCNT0 = 0;
	
	if (OCR0A>2) {
	  OCR0A--;
  }	else {
    OCR0A=255;
  }
  
	PORTD ^= (1<<LED);
}
