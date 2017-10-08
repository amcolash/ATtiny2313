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
#include <math.h>

#define PI 3.14159265

#define CLOCK PB0	//Rising edge of clock shift data one bit
#define CS	PB1		//the last 16 bits that were sent are latched on CS rising edge.
#define DATA PB2

char shape[8];

int current = 0;
int direction = 1;

float time = 0;

void increment(void) {
  current = (current + direction);
  
  if (current > 64) {
    direction = -1;
  } else if (current < 0) {
    direction = 1;
  }
  
  int tmp = current;
  for (int x = 0; x < 8; x++) {
    shape[x] = 0;
    for (int y = 0; y < 8; y++) {
      tmp--;
      if (tmp >= 0) {
        shape[x] |= 1 << y;
      } else {
        tmp = -1;
      }
    }
  }
}

void wave() {
  time = (time + 0.2);
  if (time > 2 * PI) {
    time = 0;
  }
  
  for (int x = 0; x < 8; x++) {
    shape[x] = 0;
    
    int y = sin((x / 8.0) * (2.3 * PI) + time) * 4 + 4;
    shape[x] |= 1 << y;
  }
}

void box() {
  for (int i = 0; i < current; i++) {
    for (int x = 0; x < 8; x++) {
      if (x < current || x > 7 - current) {
        shape[x] = 0b11111111;
      } else {
        shape[x] = 0;
      }
      
      for (int y = 0; y < 8; y++) {
        if (y < current || y > 7 - current) {
          shape[x] |= 1 << y;
        }
      }
    }
  }
  
  current = (current + direction);
  
  if (current > 4) {
    current = 4;
    direction = -1;
  } else if (current < 0) {
    current = 0;
    direction = 1;
  }
}

int main(void) {		
	DDRB = (1<<CLOCK) | (1<<CS) | (1<<DATA) ;

	matrix_data(0x0F,0x0);   //Display Test OFF
	matrix_data(0x0B,0x07);  //Scan Limit to digits 0 to 7.
	matrix_data(0x0C,0x1);   //Leave shutdown mode and enter normal operation

  while(1) {
    //increment();
    //wave();
    box();
  	display_image(shape);
    _delay_ms(100);
	}
	
	return 0;
}

