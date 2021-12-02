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

#include <math.h>

#include <pic32mx.h> //Chipkit specific addressess and more
#include <stdint.h>

// Status of input: 0 = listening for new message, 1 = listening for pitch, 2 =
// listening for
enum InputStatus {
  Rest,
  PitchListening,
  VelocityListening,
};

enum InputStatus status = Rest;

struct Voice {
  unsigned char channel;
  unsigned char pitch;
  unsigned char velocity;
  unsigned int wave_period;
  int sample;
};

void set_voice_pitch(unsigned char voice_index, unsigned char pitch) {

}
struct Voice voices[16];
unsigned int last_voice = 0;

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

        break;
        // Note On
      case 0x90:
        ++last_voice;
        struct Voice new_voice;
        new_voice.channel = (message & 0xF);
        status = PitchListening;
        voices[last_voice] = new_voice;
        break;
      default:
        break;
      }
    }
  } else if (status == PitchListening) {
    voices[last_voice].pitch = (message & 0x7F);
    status = VelocityListening;
  } else if (status == VelocityListening) {
    voices[last_voice].velocity = (message & 0x7F);
    status = Rest;
  }
}

// Handle interrupt and read UART input
void reciever_isr(void) {
  // Should always be called when this function is, but put here as start
  if ((IFS(1) & 0x2000000)) {
    // Read all bytes in buffer in loop?
    unsigned char input = (U1RXREG & 0xFF); // Read reciever register
    /*
     Save or return read input.
     */
    
    translate_message(input);

    IFSCLR(1) = 0x2000000; // Clear interrupt flag
  }
}

// Initialize UART configuration to handle input
void init_pin(void) {
  U2BRG = 159; // Setting the Baud rate (Assumed SYSCLK AT 80Mhz =>
               // (80000000/31250/16 -1))
  U2MODE = 0;  // RESET UART1
  U2STA = 0;   // RESET UART Status

  // 8 bit data, no parity bit, 1 stop bit so U1MODE 2:0 = 000 (PDSEL&STSEL)

  IFSCLR(1) = 0x2000000; // Clear interrupt flag for the U1 reciever
  IECSET(1) = 0x2000000; // Enable recieve interrupts at IEC0 bit 27 (U1RXIE)
  IPCCLR(8) = 0x1F;      // CLEAR interrupt priorities
  IPCSET(8) = 0b1111;    // Set priority to 3 sub priority 3 (???)
  U2STASET = 0x1000; // Set URXEN bit for reciever mode (12th bit of U1STASET)
  U2MODE = 0x8000;   // Enable UART1

  // MAKE UR1SEL Bits 00 to generate interupt for each recieved character
  U2STACLR = 0xC0;
}
