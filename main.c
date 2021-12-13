/* main.c
 *
 * Project main file
 *
 * Written by Malte Blomqvist and Isak Larsson
 *
 * Modified 2021-12-13 by Isak Larsson
 *
 */

#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h>
#include "global.h"

// For midi errors
int message = 0xFF;

// Sound Generation
void T2_IntHandler (void) {
	int output = 0;
	for (int i = 0; i < 8; i++){
		if(get_voice_velocities(i) > 0){
			// Replace saw with triangle or square at will
			int note = next_square_note(i); //SawTooth sample from voice i which is on
			//int note = next_saw_note(i);
			//int note = next_triangle_note(i);

			output |= note;
		}
	}
	
	// Clip sound, max is 8-bits (255)
	if (output > 255) {
		output = 255;
	} else if (output < 0 ) {
		output = 0;
	}

	PORTE = output;
	IFSCLR(0) = 0x0100; // Clearing Timer2 interrupt flag
}

/* Interrupt Service Routine */
void user_isr(void) {

	if (IFS(0) & 0x0100){
	T2_IntHandler();
	}

	//IF UART ERROR DO LIGHTS
	if(IFS(1) & (1 << 8)){
		IFSCLR(1) = (1 << 8);
		PORTESET = 0xFF;
	}

	if(U2STA & 2){
		U2STACLR = 2;
	}
}

void timer2_setup(void) {
	// Timer 2 setup, controlls sample-rate
	PR2 = 80000000/SAMPLE_RATE; // Set period 
	IFSCLR(0) = 0x00000100; // Clear the T2 interrupt flag
	IECSET(0) = 0x00000100; // Enable T2 interrupt
	IPCSET(2) = 0x0000001C; // Set T2 interrupt priority to 7
	T2CONSET = 0x8000; // Enable Timer2

	// Interrupt init
	enable_interrupt();

}

int main() {
	// init PORTE for sound output
	TRISE &= 0xFF00;
	initialize_pbclock();
	init_pin();
	timer2_setup();
	while (1)
	{
		/* code */
		while(!(U2STA & 0x1)); //wait for read buffer to have a value
		message = U2RXREG & 0xFF;
		translate_message(message);	
	}
	
	return 0;
}
