// Piano.c
// Runs on LM4F120 or TM4C123, 
// edX lab 13 
// There are four keys in the piano
// Daniel Valvano
// December 29, 2014

// Port E bits 3-0 have 4 piano keys

#include "Piano.h"
#include "..//tm4c123gh6pm.h"

#define C 4777
#define D 4256
#define E 3792
#define G 3189

// **************Piano_Init*********************
// Initialize piano key inputs
// Input: none
// Output: none
void Piano_Init(void){ volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;           // Port E clock
  delay = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles
  GPIO_PORTE_DIR_R &= ~0x0F;        // PE2,1,0 input
  GPIO_PORTE_AFSEL_R &= ~0x0F;      // not alternative
  GPIO_PORTE_AMSEL_R &= ~0x0F;      // no analog
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // bits for PE2,PE1,PE0
  GPIO_PORTE_DEN_R |= 0x0F;         // enable PE2,PE1,PE0
}
// **************Piano_In*********************
// Input from piano key inputs
// Input: none 
// Output: 0 to 15 depending on keys
// 0x01 is key 0 pressed, 0x02 is key 1 pressed,
// 0x04 is key 2 pressed, 0x08 is key 3 pressed
unsigned long Piano_In(void){
	switch (GPIO_PORTE_DATA_R&0x0F) {
			case 1:
				return C;
			case 2:
				return D;
			case 4:
				return E;
			case 8:
				return G;
			default:
				return 0;
	}
}