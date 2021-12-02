#include <stdint.h>  /* Declarations of uint_32 and the like */

#include <math.h>
#include <pic32mx.h>
#include "global.h"

int sample = 0;
double frequency = 220.0;

//int sine_Lookup[64] = {0x19,0x1b,0x1e,0x20,0x23,0x25,0x27,0x29, 0x2b,0x2c,0x2e,0x2f,0x30,0x31,0x32,0x32, 0x32,0x32, 0x32,0x31,0x30,0x2f,0x2e,0x2c, 0x2b,0x29,0x27,0x25,0x23,0x20,0x1e,0x1b, 0x19,0x17, 0x14,0x12,0xf, 0xd,0xb,0x9, 0x7,0x6,0x4,0x3,0x2,0x1,0x0,0x0, 0x0,0x0,0x0,0x1,0x2,0x3,0x4,0x6, 0x7,0x9,0xb,0xd,0xf,0x12,0x14,0x17};

void gen_sine(void) {
	sample++;
	//double y = sin( (2.0*M_PI*frequency*sample) / (double)SAMPLE_RATE )/2 + 0.5;
	//OC1RS = (int)round(y*255);
	OC1RS = sine_sample[sample]; // divide by 2 for 50% max
	
	
}

void gen_two_sine(void) {
	sample++;
	//double y = sin( (2.0*M_PI*frequency*sample) / (double)SAMPLE_RATE )/2 + 0.5;
	//OC1RS = (int)round(y*255);
	int adjusted = sine_sample[sample] - 127;
	int adjusted_2 = sine_sample[((sample*3)>>1)%256] - 127;
	OC1RS = ((adjusted + adjusted_2)>>1) + 127; // divide by 2 for 50% max
	if (sample == 255){
		sample = -1;
	}
}


void gen_saw(void){

}
void gen_squarewave(void) {
	
}
// Example code for Timer2 ISR:
void T2_IntHandler (void) {
	gen_two_sine();

	// Insert user code here
	IFSCLR(0) = 0x0100; // Clearing Timer2 interrupt flag
}

/* Interrupt Service Routine */
void user_isr(void) {
	T2_IntHandler();
}

void PWM_setup(void) {
	// PWM setup
	OC1CON = 0x0000; // Turn off the OC1 when performing the setup
	OC1R = sine_sample[0]; // Initialize primary Compare register to first sample
	OC1RS = sine_sample[0]; // Initialize secondary Compare register to first sample
	OC1CON = 0x000e; // Configure for PWM mode without Fault pin enabled and TM3 as controlling timer
	OC1CONSET = 0x8000; // Enable OC1

	// Timer 3 setup, controls PWM resolution
	// 312 500
	T3CON = 0x0;
  	TMR3 = 0x0;
	PR3 = 255; // Period, determies duty-cycle resolution
	//T3CONSET = 0x70;
	T3CONSET = 0x8000;

	// Timer 2 setup, controlls sample-rate
	PR2 = 80000000/SAMPLE_RATE; // Set period 
	//T2CON = 0x70;
	IFSCLR(0) = 0x00000100; // Clear the T2 interrupt flag
	IECSET(0) = 0x00000100; // Enable T2 interrupt
	IPCSET(2) = 0x0000001C; // Set T2 interrupt priority to 7
	//T2CONSET = 0x70;   // Set scalar to 1:256
	T2CONSET = 0x8000; // Enable Timer2
	

	

	// Interrupt init
	enable_interrupt();


}

int main() {
	PWM_setup();

	while (1)
	{
		/* code */
	}
	
	return 0;
}
