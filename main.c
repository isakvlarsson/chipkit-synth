#include <stdint.h>  /* Declarations of uint_32 and the like */

#include <math.h>
#include <pic32mx.h>
#include "global.h"

// For midi errors
int message = 0xFF;
int stuff = 0;
int sample = 0;

void gen_sine(void) {
	sample++;
	OC1RS = get_wavetable_sample(sample, 0); // divide by 2 for 50% max
	if (sample == 255){
		sample = -1;
	}
	
}

// Sound Generation
void T2_IntHandler (void) {
	int output = 0;
	for (int i = 0; i < 8; i++){
		if(get_voice_velocities(i) > 0){
			// Replace saw with triangle or square at will
			int note = next_square_note(i); //SawTooth sample from voice i which is on
			//int note = next_saw_note(i);
			//int note = next_triangle_note(i);
			//TODO! Blend or output notes
			//output += (note-127)/8;
			output |= note;
		}
	}
	//output += 127;
	// Clip sound, max is 8-bits (255)
	if (output > 255) {
		output = 255;
	} if (output < 0 ) {
		output = 0;
	}
	//else if< (output < 0) {
	//	output = 0;
	//}
	PORTE = output;
	IFSCLR(0) = 0x0100; // Clearing Timer2 interrupt flag
}

/* Interrupt Service Routine */
void user_isr(void) {

	if (IFS(0) & 0x0100){
	T2_IntHandler();
	}

	////IF UART ERROR DO LIGHTS
	//if(IFS(1) & (1 << 8)){
	//	IFSCLR(1) = (1 << 8);
	//	PORTESET = 0xFF;
	//}
//
	if(U2STA & 2){
		U2STACLR = 2;
	}
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
	initialize_pbclock();
	init_pin();
	PWM_setup();
	while (1)
	{
		/* code */
		while(!(U2STA & 0x1)); //wait for read buffer to have a value
		message = U2RXREG & 0xFF;
		translate_message(message);	
	}
	
	return 0;
}
