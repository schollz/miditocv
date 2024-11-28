#ifndef LIB_LFO
#define LIB_LFO

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "noise.h"
#include "slew.h"

// Define the possible LFO types
typedef enum {
  LFO_SINE,
  LFO_TRIANGLE,
  LFO_SAWTOOTH,
  LFO_SQUARE,
  LFO_DRUNK,
} LFO_Type;

// LFO struct definition
typedef struct {
  LFO_Type current_type;
  LFO_Type target_type;
  float period_ms;
  float min_val;
  float max_val;
  float phase;
  float transition_start_time;
  int is_transitioning;
} LFO;

// Constants
#define LFO_TRANSITION_DURATION_MS 3000.0f

// Triangle wave function
float triangle_wave(float t_ms, float period_ms, float min_val, float max_val,
                    float phase) {
  float t = t_ms / 1000.0f;
  float period = period_ms / 1000.0f;
  float value = (2.0f / M_PI) * asinf(sinf(2.0f * M_PI * (t / period) + phase));
  value = min_val + (max_val - min_val) * (value + 1.0f) / 2.0f;
  return value;
}

// Sine wave function
float sine_wave(float t_ms, float period_ms, float min_val, float max_val,
                float phase) {
  float t = t_ms / 1000.0f;
  float period = period_ms / 1000.0f;
  float value = sinf(2.0f * M_PI * (t / period) + phase);
  value = min_val + (max_val - min_val) * (value + 1.0f) / 2.0f;
  return value;
}

// Sawtooth wave function
float sawtooth_wave(float t_ms, float period_ms, float min_val, float max_val,
                    float phase) {
  float t = t_ms / 1000.0f;
  float period = period_ms / 1000.0f;
  float value = 2.0f * (t / period - floorf(t / period + 0.5f));
  value = min_val + (max_val - min_val) * (value + 1.0f) / 2.0f;
  return value;
}

// Square wave function
float square_wave(float t_ms, float period_ms, float min_val, float max_val,
                  float phase) {
  float t = t_ms / 1000.0f;
  float period = period_ms / 1000.0f;
  float value =
      (sinf(2.0f * M_PI * (t / period) + phase) > 0.0f) ? 1.0f : -1.0f;
  value = min_val + (max_val - min_val) * (value + 1.0f) / 2.0f;
  return value;
}

float drunk_wave(float t_ms, float period_ms, float min_val, float max_val,
                 float phase, Noise *noise, Slew *slew) {
  // check if t_ms mod period_ms is 0
  if ((int)t_ms % (int)period_ms == 0) {
    // generate random number between -1 and 1
    float target = (float)frand2(noise);
    Slew_set_target(slew, target);
    Slew_set_duration(slew, period_ms);
  }
  Slew_process2(slew, t_ms);
  return ((slew->current_value + 1.0f) / 2.0f) * (max_val - min_val) + min_val;
}
// Get LFO value based on type
float get_lfo_value(LFO_Type type, float t_ms, float period_ms, float min_val,
                    float max_val, float phase, Noise *noise, Slew *slew) {
  switch (type) {
    case LFO_TRIANGLE:
      return triangle_wave(t_ms, period_ms, min_val, max_val, phase);
    case LFO_SINE:
      return sine_wave(t_ms, period_ms, min_val, max_val, phase);
    case LFO_SAWTOOTH:
      return sawtooth_wave(t_ms, period_ms, min_val, max_val, phase);
    case LFO_SQUARE:
      return square_wave(t_ms, period_ms, min_val, max_val, phase);
    case LFO_DRUNK:
      return drunk_wave(t_ms, period_ms, min_val, max_val, phase, noise, slew);
    default:
      return 0.0f;
  }
}

#endif
