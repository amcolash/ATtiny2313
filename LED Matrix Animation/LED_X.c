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

typedef int bool;
#define true 1
#define false 0

#define PI 3.14159265

#define CLOCK PB0	//Rising edge of clock shift data one bit
#define CS	PB1		//the last 16 bits that were sent are latched on CS rising edge.
#define DATA PB2


/*#define INCREMENT*/
/*#define WAVE*/
/*#define box*/
/*#define HEARTBEAT*/
/*#define WAVE2*/
#define ROTATION


char shape[8];

int current = 0;
int direction = 1;

float time = 0;

#ifdef HEARTBEAT
char heartbeat_anim[5][8] = {
  {
    0B00000000,
    0B00000000,
    0B00000000,
    0B00001000,
    0B11110111,
    0B00000000,
    0B00000000,
    0B00000000
  },{
    0B00000000,
    0B00000000,
    0B00000000,
    0B00100000,
    0B11011011,
    0B00000100,
    0B00000000,
    0B00000000
  },{
    0B00001000,
    0B00001000,
    0B00001000,
    0B00001000,
    0B11110111,
    0B00000000,
    0B00000000,
    0B00000000
  },{
    0B00000000,
    0B00000000,
    0B00000000,
    0B00000000,
    0B11101111,
    0B00010000,
    0B00000000,
    0B00000000
  },{
    0B00000000,
    0B00000000,
    0B00000000,
    0B00001000,
    0B11110101,
    0B00000010,
    0B00000000,
    0B00000000
  }
};
#endif

#ifdef INCREMENT
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
#endif

#ifdef WAVE
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
#endif

#ifdef BOX
void box() {  
  current += direction;
  
  if (current > 4) {
    current = 4;
    direction = -1;
  } else if (current < -1) {
    current = 0;
    direction = 1;
  }

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
}
#endif

#ifdef HEARTBEAT
void heartbeat() {
  for (int x = 0; x < 8; x++) {
    shape[x] = heartbeat_anim[current][x];
  }
  
  current = (current + 1) % 5;
}
#endif

#ifdef WAVE2
wave2() {
  current = (current + 1) % 8;
  if (current == 0) {
    for (int x = 0; x < 8; x++) {
        shape[x] = 0;    
    }
  }  
  
  time = (time + 0.2);
  if (time > 2 * PI) {
    time = 0;
  }
  
  shape[current] = 0;
  
  int y = sin((current / 8.0) * (2.3 * PI) + time) * 4 + 4;
  shape[current] |= 1 << y;
}
#endif


#ifdef ROTATION
void rotation() {

  for (int i = 0; i < 8; i++) {
    shape[i] = 0;
  }

  time = time + 0.2;
  if (time > 6.0) {
    time = 0;
  }
  
/*  int x = cos(time) + 4;*/
/*  int y = sin(time) + 4;*/

  // SOMETHING IS SEVERLY WRONG HERE :( floats are messed up

  int x = ((int) time) + 3;
  int y = ((int) time) + 3;

  plot(x, y);
  
/*  drawLine((int) 3, (int) 3, (int) x, (int) y);*/
}
#endif

bool yLonger;
int incrementVal, endVal;
int shortLen, longLen;
float decInc;
float j;

void drawLine(int x, int y, int x2, int y2) {
  x = clamp(x, -1, 8);
  y = clamp(y, -1, 8);
  x2 = clamp(x2, -1, 8);
  y2 = clamp(y2, -1, 8);

  yLonger=false;
	incrementVal, endVal;

	shortLen=y2-y;
	longLen=x2-x;
	if (abs(shortLen)>abs(longLen)) {
		int swap=shortLen;
		shortLen=longLen;
		longLen=swap;
		yLonger=true;
	}
	
	endVal=longLen;
	if (longLen<0) {
		incrementVal=-1;
		longLen=-longLen;
	} else incrementVal=1;

	if (longLen==0) decInc=(float)shortLen;
	else decInc=((float)shortLen/(float)longLen);
	j=0.0;
	if (yLonger) {
		for (int i=0;i!=endVal;i+=incrementVal) {
			plot(x+(int)j,y+i);
			j+=decInc;
		}
	} else {
		for (int i=0;i!=endVal;i+=incrementVal) {
			plot(x+i,y+(int)j);
			j+=decInc;
		}
	}
}

int clamp(int d, int min, int max) {
  const int t = d < min ? min : d;
  return t > max ? max : t;
}

void plot(int x, int y) {
  shape[x] |= 1 << y;
}

int main(void) {		
	DDRB = (1<<CLOCK) | (1<<CS) | (1<<DATA);

	matrix_data(0x0F,0x0);   //Display Test OFF
	matrix_data(0x0B,0x07);  //Scan Limit to digits 0 to 7.
	matrix_data(0x0C,0x1);   //Leave shutdown mode and enter normal operation

  while(1) {
    #ifdef INCREMENT
    increment();
    #endif
    
    #ifdef WAVE
    wave();
    #endif
    
    #ifdef BOX
    box();
    #endif
    
    #ifdef HEARTBEAT
    heartbeat();
    #endif
    
    #ifdef WAVE2
    wave2();
    #endif
    
    #ifdef ROTATION
    rotation();
    #endif
    
  	display_image(shape);
  	
    //_delay_ms(150);
    //_delay_ms(150);
	}
	
	return 0;
}

