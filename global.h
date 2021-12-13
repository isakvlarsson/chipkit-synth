/* global.h
 *
 * Header file for global definitions
 *
 * Written by Malte Blomqvist
 *
 * Modified 2021-12-13 by Isak Larsson
 *
 */

// For interrupt handling
void enable_interrupt(void);
void receiver_isr(void);

// Global variables
#define SAMPLE_RATE 80000 // (80_000_000/10_000)
#define TIME_PER_SAMPLE 1/SAMPLE_RATE

// For voices.c tracking voices and their velocities 
void set_voice_velocities(int i, int value);
void set_voice_pitch(int i, int value);
int get_voice_velocities(int i);
int get_voice_pitch(int i);

// For voices.c generating sound 
int next_saw_note(int voice);
int next_triangle_note(int voice);
int next_square_note(int voice);

//message parsing
void translate_message(unsigned char message);


void init_pin(void);

void initialize_pbclock(void);
