#ifndef LIB_LFO
#define LIB_LFO

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "noise.h"

// Define the possible LFO types
typedef enum {
  LFO_TRIANGLE,
  LFO_SINE,
  LFO_SAWTOOTH,
  LFO_SQUARE,
  LFO_DRUNK,
} LFO_Type;

// LFO struct definition
typedef struct {
  LFO_Type type;
  float period_ms;
  float min_val;
  float max_val;
  float phase;
  Noise noise;
} LFO;

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

// LFO update function
float LFO_update(LFO *lfo, float ct) {
  switch (lfo->type) {
    case LFO_TRIANGLE:
      return triangle_wave(ct, lfo->period_ms, lfo->min_val, lfo->max_val,
                           lfo->phase);
    case LFO_SINE:
      return sine_wave(ct, lfo->period_ms, lfo->min_val, lfo->max_val,
                       lfo->phase);
    case LFO_SAWTOOTH:
      return sawtooth_wave(ct, lfo->period_ms, lfo->min_val, lfo->max_val,
                           lfo->phase);
    case LFO_SQUARE:
      return square_wave(ct, lfo->period_ms, lfo->min_val, lfo->max_val,
                         lfo->phase);
    case LFO_DRUNK:
      return LFNoise2(&lfo->noise, ct, lfo->period_ms / 1000.0, lfo->min_val,
                      lfo->max_val);
    default:
      return 0.0f;
  }
}

void LFO_set_type(LFO *lfo, LFO_Type type) { lfo->type = type; }

void LFO_init(LFO *lfo, LFO_Type type, float period_ms, float min_val,
              float max_val, float phase) {
  lfo->type = type;
  lfo->period_ms = period_ms;
  lfo->min_val = min_val;
  lfo->max_val = max_val;
  lfo->phase = phase;
  Noise_init(&lfo->noise, 0);
}

#endif