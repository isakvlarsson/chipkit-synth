#include <stdint.h>  /* Declarations of uint_32 and the like */
#include <pic32mx.h>
#include "global.h"

int PWM = 0;
// Example code for Timer2 ISR:
void T2_IntHandler (void) {
	if (PWM) {
		OC1RS = 0x0;
		PWM = 0;
	} else {
		OC1RS = 0x0000;
		PWM = 1;
	}
	
	// Insert user code here
	IFSCLR(0) = 0x0100; // Clearing Timer2 interrupt flag
}

/* Interrupt Service Routine */
void user_isr(void) {
	T2_IntHandler();
}

int main() {
	
	OC1CON = 0x0000; // Turn off the OC1 when performing the setup
	OC1R = 0x0064; // Initialize primary Compare register
	OC1RS = 0x0064; // Initialize secondary Compare register
	OC1CON = 0x000e; // Configure for PWM mode without Fault pin enabled
	PR2 = 31250; // Set period
	IFSCLR(0) = 0x00000100; // Clear the T2 interrupt flag
	IECSET(0) = 0x00000100; // Enable T2 interrupt
	IPCSET(2) = 0x0000001C; // Set T2 interrupt priority to 7
	T2CONSET = 0x8000; // Enable Timer2
	T2CONSET = 0x70;   // Set scalar to 1:256
	OC1CONSET = 0x8000; // Enable OC1

	// Interrupt init
	enable_interrupt();

	while (1)
	{
		/* code */
	}
	
	return 0;
}
