// ***** 0. Documentation Section *****
// SwitchLEDInterface.c for Lab 8
// Runs on LM4F120/TM4C123
// Use simple programming structures in C to toggle an LED
// while a button is pressed and turn the LED on when the
// button is released.  This lab requires external hardware
// to be wired to the LaunchPad using the prototyping board.
// January 15, 2016
//      Jon Valvano and Ramesh Yerraballi

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****
unsigned long SW1;

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortE_Init(void); //Port E initialization
void Delay1ms(unsigned long msec); //Delay 1 ms

// ***** 3. Subroutines Section *****

// PE0, PB0, or PA2 connected to positive logic momentary switch using 10k ohm pull down resistor
// PE1, PB1, or PA3 connected to positive logic LED through 470 ohm current limiting resistor
// To avoid damaging your hardware, ensure that your circuits match the schematic
// shown in Lab8_artist.sch (PCB Artist schematic file) or 
// Lab8_artist.pdf (compatible with many various readers like Adobe Acrobat).
int main(void){ 
//**********************************************************************
// The following version tests input on PE0 and output on PE1
//**********************************************************************
  TExaS_Init(SW_PIN_PE0, LED_PIN_PE1, ScopeOn);  // activate grader and set system clock to 80 MHz
  PortE_Init();
  EnableInterrupts();           // enable interrupts for the grader
	GPIO_PORTE_DATA_R = 0x02;
  while(1){
		Delay1ms(100);
		SW1 = GPIO_PORTE_DATA_R&0x01;
		if (SW1) {
			if ((GPIO_PORTE_DATA_R&0x02) == 0x02) GPIO_PORTE_DATA_R = 0x00;
			else GPIO_PORTE_DATA_R = 0x02;
		} else GPIO_PORTE_DATA_R = 0x02;
  }
  
}

void PortE_Init(void) { volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;           // Port E clock
  delay = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles
  GPIO_PORTE_DIR_R |= 0x02;         // PE1 output
  GPIO_PORTE_DIR_R &= ~0x01;        // PE0 input
  GPIO_PORTE_AFSEL_R &= ~0x03;      // not alternative
  GPIO_PORTE_AMSEL_R &= ~0x03;      // no analog
  //GPIO_PORTE_PCTL_R &= ~0x000000FF; // bits for PE1,PE0
  GPIO_PORTE_DEN_R |= 0x03;         // enable PE1,PE0
}

void Delay1ms(unsigned long msec){
// write this function
  unsigned long i;
  while(msec > 0){
    i = 13333;  // this number means 1ms
    while(i > 0){
      i = i - 1;
    }
    msec = msec - 1; // decrements every 1 ms
  }
}
