/* voices.c
 *
 * File for keeping track of the voices and their states.
 * This is where sound is generated
 * 
 * Written by Malte Blomqvist
 *
 * Last updated 2021-12-13 by Isak Larsson
 *
 * For copyright and licensing, see file COPYING
 */


#include <math.h>
#include <stdint.h>
#include "global.h"
const int SAMPLES_PER_PERIOD[] = {
    9785, 9236, 8717, 8228, 7766, 7330, 6919, 6531, 6164, 5818, 5492, 5183,
    4892, 4618, 4359, 4114, 3883, 3665, 3460, 3265, 3082, 2909, 2746, 2592,
    2446, 2309, 2179, 2057, 1942, 1833, 1730, 1633, 1541, 1455, 1373, 1296,
    1223, 1154, 1090, 1029, 971,  916,  865,  816,  771,  727,  686,  648,
    612,  577,  545,  514,  485,  458,  432,  408,  385,  364,  343,  324,
    306,  289,  272,  257,  243,  229,  216,  204,  193,  182,  172,  162,
    153,  144,  136,  129,  121,  115,  108,  102,  96,   91,   86,   81,
    76,   72,   68,   64,   61,   57,   54,   51,   48,   45,   43,   40,
    38,   36,   34,   32,   30,   29,   27,   26,   24,   23,   21,   20,
    19,   18,   17,   16,   15,   14,   14,   13,   12,   11,   11,   10,
    10,   9,    9,    8,    8,    7,    7};

const int MAX_AMPLITUDE = 255;

int voice_pitch[] = {0,0,0,0,0,0,0,0};

int voice_counters[] = {0, 0, 0, 0, 0, 0, 0, 0};

int voice_velocities[] = {0, 0, 0, 0, 0, 0, 0, 0};

/// Setters and getters
void set_voice_velocities(int i, int value){
  voice_velocities[i] = value;
}
void set_voice_pitch(int i, int value){
  voice_pitch[i] = value;
}
int get_voice_velocities(int i){
  return voice_velocities[i];
}
int get_voice_pitch(int i){
  return voice_pitch[i];
}

// Generates the next sample of a saw wave
int next_saw_note(int voice) {
  int sample = voice_counters[voice];
  int pitch = voice_pitch[voice];
  int s_period = SAMPLES_PER_PERIOD[pitch];
  int amplitude = voice_velocities[voice];

  int note = round(amplitude * ((float)sample / s_period));

  if (sample >= s_period) {
    sample = 0;
  } else {
    sample++;
  }
  voice_counters[voice] = sample;
  return note;
}

// Generates the next sample of a triangle wave
int next_triangle_note(int voice) {
  int sample = voice_counters[voice];
  int pitch = voice_pitch[voice];
  int s_period = SAMPLES_PER_PERIOD[pitch];
  int half_period = s_period >> 1;
  int amplitude = voice_velocities[voice];

  int note;
  if (sample > half_period) {
    note = round(amplitude * (1 - ((float)sample / half_period)));
  } else {
    note = round(amplitude * ((float)sample / half_period));
  }

  if (sample >= s_period) {
    sample = 0;
  } else {
    sample++;
  }
  voice_counters[voice] = sample;
  return note;
}

// Generates the next sample of a square wave
int next_square_note(int voice) {
  int sample = voice_counters[voice];
  int pitch = voice_pitch[voice];
  int s_period = SAMPLES_PER_PERIOD[pitch];
  int half_period = s_period >> 4; // Higher divisor seems to increase tone clarity
  int amplitude = voice_velocities[voice];

  int note;

  if (sample > half_period) {
    note = 0;
  } else {
    note = amplitude;
  }

  if (sample >= s_period) {
    sample = 0;
  } else {
    sample++;
  }
  voice_counters[voice] = sample;
  return note;
}
