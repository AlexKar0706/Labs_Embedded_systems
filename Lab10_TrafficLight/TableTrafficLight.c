// ***** 0. Documentation Section *****
// TableTrafficLight.c for Lab 10
// Runs on LM4F120/TM4C123
// Index implementation of a Moore finite state machine to operate a traffic light.  
// Daniel Valvano, Jonathan Valvano
// January 15, 2016

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0
// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)
// "walk" light connected to PF3 (built-in green LED)
// "don't walk" light connected to PF1 (built-in red LED)

// ***** 1. Pre-processor Directives Section *****
#include "TExaS.h"
#include "tm4c123gh6pm.h"

// ***** 2. Global Declarations Section *****

// FUNCTION PROTOTYPES: Each subroutine defined
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts
void PortB_Init(void);  // Port B initialization
void PortE_Init(void);  // Port E initialization
void PortF_Init(void);  // Port F initialization
void SysTick_Init(void);
void SysTick_Wait10ms(long delay);
void ledFlashing(void);

#define goW 0
#define waitW 1
#define goS 2
#define waitS 3
#define walk 4
#define hurry 5
#define off_1 6
#define blink_1 7
#define off_2 8
#define blink_2 9
#define off_3 10

struct State {
  unsigned long Out[2]; 
  unsigned long Time;
  unsigned long Next[8];
};
typedef struct State stateArr;

stateArr FSM[11] = {
	{{0x0C, 0x02}, 15, {goW, goW, waitW, waitW, waitW, waitW, waitW, waitW}},      //GoWest
	{{0x14, 0x02}, 15, {goW, goW, goS, goS, walk, walk, goS, goS}},            		//WaitWest
	{{0x21, 0x02}, 15, {goS, waitS, goS, waitS, waitS, waitS, waitS, waitS}},      //GoSouth
	{{0x22, 0x02}, 15, {goS, goW, goS, goW, walk, walk, walk, walk}},				  		//WaitSouth
	{{0x24, 0x08}, 15, {walk, hurry, hurry, hurry, walk, hurry, hurry, hurry}},    //Walk
	{{0x24, 0x00}, 15, {off_1, off_1, off_1, off_1, off_1, off_1, off_1, off_1}},  //Lights off for blink 1
	{{0x24, 0x02}, 15, {blink_1, blink_1, blink_1, blink_1, blink_1, blink_1, blink_1, blink_1}},  //Blink 1
	{{0x24, 0x00}, 15, {off_2, off_2, off_2, off_2, off_2, off_2, off_2, off_2}},  //Lights off for blink 2
	{{0x24, 0x02}, 15, {blink_2, blink_2, blink_2, blink_2, blink_2, blink_2, blink_2, blink_2}},  //Blink 2
	{{0x24, 0x00}, 15, {off_3, off_3, off_3, off_3, off_3, off_3, off_3, off_3}},  //Lights off for blink 3
	{{0x24, 0x02}, 15, {walk, waitW, waitS, waitW, walk, waitW, waitS, waitW}}, //Blink 3
};

unsigned long S;
unsigned long Input;

// ***** 3. Subroutines Section *****

int main(void){ 
  TExaS_Init(SW_PIN_PE210, LED_PIN_PB543210,ScopeOff); // activate grader and set system clock to 80 MHz
	PortF_Init();
	PortB_Init();
  PortE_Init();
	SysTick_Init();
  EnableInterrupts();
	S = goW;
  while(1){
    GPIO_PORTB_DATA_R = FSM[S].Out[0];  // set lights on PORT B LED's
		GPIO_PORTF_DATA_R = FSM[S].Out[1];  // set lights on PORT F LED's
    SysTick_Wait10ms(FSM[S].Time);
    Input = GPIO_PORTE_DATA_R;     // read sensors
		S = FSM[S].Next[Input]; 
  }
}

void PortE_Init(void) { volatile unsigned long delay;
	SYSCTL_RCGC2_R |= 0x10;           // Port E clock
  delay = SYSCTL_RCGC2_R;           // wait 3-5 bus cycles
  GPIO_PORTE_DIR_R &= ~0x07;        // PE2,1,0 input
  GPIO_PORTE_AFSEL_R &= ~0x07;      // not alternative
  GPIO_PORTE_AMSEL_R &= ~0x07;      // no analog
  GPIO_PORTE_PCTL_R &= ~0x00000FFF; // bits for PE2,PE1,PE0
  GPIO_PORTE_DEN_R |= 0x07;         // enable PE2,PE1,PE0
}

void PortF_Init(void){ volatile unsigned long delay;
  SYSCTL_RCGC2_R |= 0x20;            // 1) F clock
  delay = SYSCTL_RCGC2_R;            // delay to allow clock to stabilize     
  GPIO_PORTF_AMSEL_R &= 0x00;        // 2) disable analog function
  GPIO_PORTF_PCTL_R &= 0x00000000;   // 3) GPIO clear bit PCTL  
  GPIO_PORTF_DIR_R |= 0x0A;          // 4) PF3,1 output  
  GPIO_PORTF_AFSEL_R &= 0x00;        // 5) no alternate function       
  GPIO_PORTF_DEN_R |= 0x0A;          // 7) enable digital pins PF3, PF1
}

void PortB_Init(void){ volatile long delay;
	SYSCTL_RCGC2_R |= 0x02;          // activate Port B
  delay = SYSCTL_RCGC2_R;          // allow time for clock to stabilize
  GPIO_PORTB_AMSEL_R &= 0x00;        // disable analog function
	GPIO_PORTB_PCTL_R &= 0x00000000;   // GPIO clear bit PCTL
  GPIO_PORTB_DIR_R |= 0x3F;         // make PB5-0 out
  GPIO_PORTF_AFSEL_R &= 0x00;        //no alternate functio
  GPIO_PORTB_DEN_R |= 0x3F;         // enable digital I/O on PB5-0
}

void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;               // disable SysTick during setup
	NVIC_ST_RELOAD_R = 0x00FFFFFF;   // maximum reload value
  NVIC_ST_CURRENT_R = 0;           // any write to current clears it
  NVIC_ST_CTRL_R = 0x05;      // enable SysTick with core clock
}

// The delay parameter is in units of the 80 MHz core clock. (12.5 ns)

void SysTick_Wait(long delay){

  NVIC_ST_RELOAD_R = delay-1;  // number of counts to wait

  NVIC_ST_CURRENT_R = 0;       // any value written to CURRENT clears

  while((NVIC_ST_CTRL_R&0x00010000)==0){ // wait for count flag

  }

}

// 800000*12.5ns equals 10ms

void SysTick_Wait10ms(long delay){

  long i;

  for(i=0; i<delay; i++){
    SysTick_Wait(800000);  // wait 10ms
  }
}
