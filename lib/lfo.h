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
  LFO_Type current_type;
  LFO_Type target_type;
  float period_ms;
  float min_val;
  float max_val;
  float phase;
  Noise noise;
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

// Get LFO value based on type
float get_lfo_value(LFO_Type type, float t_ms, float period_ms, float min_val,
                    float max_val, float phase, Noise *noise) {
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
      return LFNoise2(noise, t_ms, period_ms / 1000.0f, min_val, max_val);
    default:
      return 0.0f;
  }
}

// LFO update function with transition support
float LFO_update(LFO *lfo, float current_time_ms) {
  // Check if transitioning
  if (lfo->is_transitioning) {
    float elapsed_time = current_time_ms - lfo->transition_start_time;
    if (elapsed_time >= LFO_TRANSITION_DURATION_MS) {
      // End transition
      lfo->is_transitioning = 0;
      lfo->current_type = lfo->target_type;
    } else {
      // Interpolate between current and target LFO types
      float progress = elapsed_time / LFO_TRANSITION_DURATION_MS;
      float current_value =
          get_lfo_value(lfo->current_type, current_time_ms, lfo->period_ms,
                        lfo->min_val, lfo->max_val, lfo->phase, &lfo->noise);
      float target_value =
          get_lfo_value(lfo->target_type, current_time_ms, lfo->period_ms,
                        lfo->min_val, lfo->max_val, lfo->phase, &lfo->noise);
      // Linear interpolation
      return current_value * (1.0f - progress) + target_value * progress;
    }
  }

  // No transition, return current LFO value
  return get_lfo_value(lfo->current_type, current_time_ms, lfo->period_ms,
                       lfo->min_val, lfo->max_val, lfo->phase, &lfo->noise);
}

// Set LFO type with transition
void LFO_set_type(LFO *lfo, LFO_Type new_type, float current_time_ms) {
  if (lfo->current_type != new_type) {
    lfo->target_type = new_type;
    lfo->transition_start_time = current_time_ms;
    lfo->is_transitioning = 1;
  }
}

// Initialize LFO
void LFO_init(LFO *lfo, LFO_Type type, float period_ms, float min_val,
              float max_val, float phase) {
  lfo->current_type = type;
  lfo->target_type = type;
  lfo->period_ms = period_ms;
  lfo->min_val = min_val;
  lfo->max_val = max_val;
  lfo->phase = phase;
  lfo->is_transitioning = 0;
  lfo->transition_start_time = 0;
  Noise_init(&lfo->noise, 0);
}

#endif
