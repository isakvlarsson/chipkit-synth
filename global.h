// Header file for the project

void enable_interrupt(void);


// Global variables
#define SAMPLE_RATE 80000 // (80_000_000/10_000)
#define TIME_PER_SAMPLE 1/SAMPLE_RATE

// For voices.c tracking voices and their velocities 

//int voice_velocities[8];
//
//int voice_pitch[8];
void set_voice_velocities(int i, int value);
void set_voice_pitch(int i, int value);
int get_voice_velocities(int i);
int get_voice_pitch(int i);
int next_saw_note(int voice);

int next_triangle_note(int voice);

int next_square_note(int voice);

//message parsing
void translate_message(unsigned char message);

// Wavetable headers
int get_wavetable_sample(int sample, int pitch); 

int get_wavetable_period(int pitch); 

void receiver_isr(void);

void debug_note(void);

void init_pin(void);

void initialize_pbclock(void);
