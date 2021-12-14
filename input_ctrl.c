/* input_ctrl.c
 *
 * This file is to handle the handling of MIDI-in
 * Using UART
 *
 * Written by Malte Blomqvist
 *
 * Modified 2021-12-13 by Isak Larsson
 *
 */

#include "global.h"
#include <pic32mx.h> //Chipkit specific addressess and more
#include <stdint.h>
// Status of input: 0 = listening for new message, 1 = listening for pitch, 2 =
// listening for
enum InputStatus { Rest, PitchOn, VelocityOn, PitchOff, VelocityOff };

enum InputStatus status = Rest;

unsigned int last_voice = 0;
unsigned int last_statusMessage = 0xFF;
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
        last_statusMessage = 0x80;
        status = PitchOff;
        break;

        // Note On
      case 0x90:
        last_statusMessage = 0x90;
        for (int i = 0; i < 8; i++) {
          if (get_voice_velocities(i) == 0) {
            last_voice = i;
            status = PitchOn; // Find a voice that's off
            break;
          }
        }
        break;

      default:
	last_statusMessage = 0xFF;
        break;
      }
      /// Check if data byte, do last message.
    } else if (last_statusMessage == 0x80) {
      for (int i = 0; i < 8; i++) {
        if (get_voice_pitch(i) == message) {
          last_voice = i; // Find note to turn off
        }
      }
      status = VelocityOff;
    } else if (last_statusMessage == 0x90) {
      int new_voice = 0;
      for (int i = 0; i < 8; i++) {
        if (get_voice_pitch(i) == message) {
          last_voice = i;
          status = VelocityOn;
          return;
        } else if (get_voice_pitch(i) == 0) {
          new_voice = i;
        }
      }
      last_voice = new_voice;
      set_voice_pitch(last_voice, message); // set the pitch
      status = VelocityOn;
    }
  } else if (status == PitchOn) {
    for (int i = 0; i < 8; i++) {
      if (get_voice_pitch(i) == message) {
        last_voice = i;
        status = VelocityOn;
        return;
      }
    }
    set_voice_pitch(last_voice, message); // set the pitch
    status = VelocityOn;
  } else if (status == PitchOff) {
    for (int i = 0; i < 8; i++) {
      if (get_voice_pitch(i) == message) {
        last_voice = i; // Find note to turn off
      }
    }
    status = VelocityOff;
  } else if (status == VelocityOn) {
    set_voice_velocities(last_voice,
                         message); // Replace 255 with message to use custom velocity
    status = Rest;
  } else if (status == VelocityOff) {
    set_voice_velocities(last_voice, 0); // Shut Note off
    status = Rest;
  }
}

// Initialize UART configuration to handle input
// U2RX = PIN 39 on UNO SHIELD
void init_pin(void) {

  ODCF = 0;
  TRISFCLR = 0xFF;

  U2BRG = 159; // Setting the Baud rate (Assumed SYSCLK AT 80Mhz =>
               // (80000000/31250/16 -1))
  U2MODE = 0;  // RESET UART
  U2STA = 0;   // RESET UART Status
  // 8 bit data, no parity bit, 1 stop bit so U2MODE 2:0 = 000 (PDSEL&STSEL)

  U2MODESET = 0x8000; // Enable UART2

  U2STASET =
      0x1400; // Set URXEN bit (UTXEN) for reciever mode (12th bit of U2STASET)
  TRISE = 0;
  PORTE = 0;
  TRISE &= 0xFFFF; // AND with 1111111100000000 to set 8 LSB to 0
  PORTE &= 0xFF00; // AND with 1111111100000000 to set 8 LSB to 0
}
