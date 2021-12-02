// Header file for the project

void enable_interrupt(void);


// Global variables
#define SAMPLE_RATE 80000 // (80_000_000/10_000)
#define TIME_PER_SAMPLE 1/SAMPLE_RATE

// Wavetable headers
int get_wavetable_sample(int sample, int pitch); 

int get_wavetable_period(int pitch); 