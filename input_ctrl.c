/* input_ctrl.c
 *
 * This file is to handle the handling of MIDI-in
 * Using UART
 *
 * Written by M Blomqvist
 *
 * Modified 2021-11-22
 *
 */

#include <pic32mx.h> //Chipkit specific addressess and more
#include <stdint.h>
#include "global.h"
// Status of input: 0 = listening for new message, 1 = listening for pitch, 2 =
// listening for
enum InputStatus {
  Rest,
  PitchOn,
  VelocityOn,
  PitchOff,
  VelocityOff
};

enum InputStatus status = Rest;

unsigned int last_voice = 0;
unsigned char voice_flags;

// To ensure 80MHz peripheral bus clock
// Credit to mipslabmain.c by Axel Isaksson & F Lundevall
// Page 6-27 in pic32 family manual
void initialize_pbclock(void) {
  SYSKEY = 0xAA996655; /* Unlock OSCCON, step 1 */
  SYSKEY = 0x556699AA; /* Unlock OSCCON, step 2 */
  while (OSCCON & (1 << 21))
    ;                   /* Wait until PBDIV ready */
  OSCCONCLR = 0x180000; /* clear PBDIV bit <0,1> */
  while (OSCCON & (1 << 21))
    ;           /* Wait until PBDIV ready */
  SYSKEY = 0x0; /* Lock OSCCON */
}

void translate_message(unsigned char message) {
  // Status Message MSB is set 0x80 <= message <= 0xFF
  if (status == Rest) {
    if (message >= 0x80) {
      switch (message & 0xF0) {
        // Note Off
      case 0x80:	
      
	    status = PitchOff;
        break;
        // Note On
      case 0x90:
       for (int i = 0; i < 8; i++){
	       if(get_voice_velocities(i) == 0){
		       last_voice = i;
       		       status = PitchOn; 	//Find a voice that's off
		       break;
	       }
       }
        break;
      default:
        break;
      }
    }
  } else if (status == PitchOn) {
	  set_voice_pitch(last_voice, message); 	//set the pitch
    	status = VelocityOn;
  } else if (status == PitchOff) {
    for (int i = 0; i < 8; i++){
	    if (get_voice_pitch(i) == message){
		last_voice = i; 		//Find note to turn off
	    }
    }
    status = VelocityOff;
  } else if (status == VelocityOn){
	  set_voice_velocities(last_voice, 60); //Replace 255 with message to use custom velocity
	  status = Rest;
  }else if (status == VelocityOff){
	  set_voice_velocities(last_voice, 0); // Shut Note off
    status = Rest;
  }
}

// Initialize UART configuration to handle input
// U2RX = PIN 39 on UNO SHIELD
void init_pin(void) {

  ODCF = 0;
  TRISFCLR = 0xFF;

  U2BRG =159; // Setting the Baud rate (Assumed SYSCLK AT 80Mhz =>
               // (80000000/31250/16 -1))
  U2MODE = 0;  // RESET UART
  U2STA = 0;   // RESET UART Status
  // 8 bit data, no parity bit, 1 stop bit so U2MODE 2:0 = 000 (PDSEL&STSEL)

  IECSET(1) = (3 << 8); // Enable recieve interrupts at IEC1 bit 8 & 9 (U2RXIE, U2EIE)
  IPCSET(8) = 0x1E; 	// SET interrupt priorities - 3 sub 6
  IFSCLR(1) = (3 << 8); // Clear interrupt flags for the U2 reciever (U2RXIF & U2EIF)


  U2MODESET = 0x8000;   // Enable UART2

  U2STASET = 0x1400; // Set URXEN bit (UTXEN) for reciever mode (12th bit of U2STASET)
    TRISE = 0;
    PORTE = 0;
    TRISE &= 0xFFFF; // AND with 1111111100000000 to set 8 LSB to 0
    PORTE &= 0xFF00; // AND with 1111111100000000 to set 8 LSB to 0
}
