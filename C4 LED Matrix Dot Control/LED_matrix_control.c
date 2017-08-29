/*
 * MCU: ATtiny2313
 * Send 16 bits at a time: first 4 are Don't Cares. next 4 are ADDRESS. and then 8 DATA bits.
 * The 4 ADDRESS bits allow us to choose a column, and then the 8 DATA bits represent which of the 8 LEDs in that column are on/off 
 
 * However, before we can control the LEDs, we must initialize the LED Matrix:
 * Upon powering up, we're in shutdown mode and only scan 1 digit (i.e only the leftmost column can be controlled).
 * To initialize: Leave shutdown mode (i.e. enter Normal Operation mode), then set Scan Limit to digits 0 to 7. This way, we can control all the columns.
 * 16 bit instruction to enter Normal Operation: 0000 1100 0000 0001
 * 16-bit instruction to set scan limit to digits 0 to 7: 0000 1011 0000 0111  		(i.e. we control all columns when scan limit is set to digits 0 to 7)
 * Created by TronClub.com
 */

#include <avr/io.h>
#include <util/delay.h> 

#define CLOCK PB0	//Rising edge of clock shift data one bit
#define CS	PB1		//the last 16 bits that were sent are latched on CS rising edge.
#define DATA PB2



int main(void) {	
  DDRB = (1<<CLOCK) | (1<<CS) | (1<<DATA) ;
  display_test_off();		//turn off display test
  scan_limit();			    //set scan limit to digits 0 to 7, to control all columns
  shutdown();				    //leave shutdown mode, enter Normal Operation mode.
  
  clear();
  diagonal();
}


void clear() {
  send_col(0b0001, 0b00000000);
  send_col(0b0010, 0b00000000);
  send_col(0b0011, 0b00000000);
  send_col(0b0100, 0b00000000);
  send_col(0b0101, 0b00000000);
  send_col(0b0110, 0b00000000);
  send_col(0b0111, 0b00000000);
  send_col(0b1000, 0b00000000);
}

void diagonal() {
  send_col(0b0001, 0b10000001);
  send_col(0b0010, 0b01000010);
  send_col(0b0011, 0b00100100);
  send_col(0b0100, 0b00011000);
  send_col(0b0101, 0b00011000);
  send_col(0b0110, 0b00100100);
  send_col(0b0111, 0b01000010);
  send_col(0b1000, 0b10000001);
}

void send_col(int col, int values) {
  int i = 0;
  for (i = 0; i < 4; i++) {
    send_zero();
  }
  
  for (i = 3; i >= 0; i--) {
    if (col & (1 << i)) {
      send_one();
    } else {
      send_zero();
    }
  }
  
  for (i = 7; i >= 0; i--) {
    if (values & (1 << i)) {
      send_one();
    } else {
      send_zero();
    }
  }
  
  matrix_read();
}

void shutdown(void)	{	//0000 1100 0000 0001
  send_zero();
  send_zero();
  send_zero();
  send_zero();

  send_one();
  send_one();
  send_zero();
  send_zero();

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

void scan_limit(void) {		//0000 1011 0000 0111		scan digits 0 to 7
  send_zero();
  send_zero();
  send_zero();
  send_zero();

  send_one();
  send_zero();
  send_one();
  send_one();

  send_zero();
  send_zero();
  send_zero();
  send_zero();

  send_zero();
  send_one();
  send_one();
  send_one();

  matrix_read();
}

void display_test_off(void) {			//0000 1111 0000 0000. Turn off display test
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
  send_zero();

  matrix_read();
}

void send_zero(void) {
	// clear DATA and CLOCK, then set CLOCK.
  PORTB &= ~(1<<DATA | 1<<CLOCK);
  PORTB |= (1<<CLOCK); 
}

void send_one(void) {		  //set DATA and clear CLOCK, then set CLOCK. 
  PORTB = (1<<DATA);
  PORTB |= (1<<CLOCK);		//sets DATA = 1 		then sets CLOCK. 
}

void matrix_read(void) {  //Set CS as HIGH, then LOW to read the last 16 bits
  PORTB |= (1<<CS);		
  _delay_ms(2);
  PORTB &= ~(1<<CS);   
  _delay_ms(2);
}

