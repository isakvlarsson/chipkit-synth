#include <stdint.h>  /* Declarations of uint_32 and the like */

#include <math.h>
#include <pic32mx.h>
#include "global.h"


int sample_69 = 0;
int max_69 = 182;
int sample_65 = 0;
int max_65 = 229;
//int sine_Lookup[64] = {0x19,0x1b,0x1e,0x20,0x23,0x25,0x27,0x29, 0x2b,0x2c,0x2e,0x2f,0x30,0x31,0x32,0x32, 0x32,0x32, 0x32,0x31,0x30,0x2f,0x2e,0x2c, 0x2b,0x29,0x27,0x25,0x23,0x20,0x1e,0x1b, 0x19,0x17, 0x14,0x12,0xf, 0xd,0xb,0x9, 0x7,0x6,0x4,0x3,0x2,0x1,0x0,0x0, 0x0,0x0,0x0,0x1,0x2,0x3,0x4,0x6, 0x7,0x9,0xb,0xd,0xf,0x12,0x14,0x17};

int clip_255(int value) {
	if (value > 255) {
		return 255;
	} else {
		return value;
	}
	
}
void gen_sine(void) {
	sample_65++;

	//double y = sin( (2.0*M_PI*frequency*sample) / (double)SAMPLE_RATE )/2 + 0.5;
	//OC1RS = (int)round(y*255);

	PORTE = clip_255(get_wavetable_sample(sample_65, 0)>>3);
	if (sample_65 == (max_65 - 1)){
		sample_65 = -1;
	}

	
}

void gen_two_sine(void) {
	sample_69++;
	sample_65++;
	//double y = sin( (2.0*M_PI*frequency*sample) / (double)SAMPLE_RATE )/2 + 0.5;
	//OC1RS = (int)round(y*255);
	int s69 = (get_wavetable_sample(sample_69, 1))>>2;
	int s65 = (get_wavetable_sample(sample_65, 0))>>2;

	PORTE = clip_255(s69 + s65) ; // divide by 2 for 50% max
	if (sample_69 == (max_69 - 1)){
		sample_69 = -1;
	}
	if (sample_65 == (max_65 - 1)){
		sample_65 = -1;
	}
	
}
char up = 0;
void simple_audio() {
  if (PORTE == 255) {
    up = 0;
  } else if (PORTE == 0) {
    up = 1;
  }
  if (up) {
    PORTE += 1;
  } else {
    PORTE -= 1;
  }
}
void gen_saw(void){

}
void gen_two_squarewave(void) {

	sample_69++;
	sample_65++;
	//double y = sin( (2.0*M_PI*frequency*sample) / (double)SAMPLE_RATE )/2 + 0.5;
	//OC1RS = (int)round(y*255);
	int s69;
	int s65;
	if (sample_69 >= (max_69/2)) {
		s69 = 255;
	} else {
		s69 = 0;
	}
	if (sample_65 >= (max_65/2)) {
		s65 = 255;
	}else {
		s65 = 0;
	}
	
	PORTE = clip_255((s65 + s69)>>4); // divide by 2 for 50% max
	if (sample_69 == (max_69 - 1)){
		sample_69 = -1;
	}
	if (sample_65 == (max_65 - 1)){
		sample_65 = -1;
	}
	

}

void gen_square_wave(void) {
	sample_69++;

	if (sample_69 >= (max_69/2)) {
		PORTE = 0xFF - 127;
	} else {
		PORTE = 0;
	}
}
// Example code for Timer2 ISR:
void T2_IntHandler (void) {
	//gen_sine();
	//gen_two_sine();
	//simple_audio();
	gen_two_squarewave();
	//PORTESET = 0xFF;
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
	OC1R = 0x00; // Initialize primary Compare register to first sample
	OC1RS = 0x00; // Initialize secondary Compare register to first sample
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

	// init PORTE for sound output
	TRISE &= 0xFF00;
	
	PWM_setup();
	
	while (1)
	{
		/* code */
	}
	
	return 0;
}
