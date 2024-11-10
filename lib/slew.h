#ifndef SLEW_LIB
#define SLEW_LIB 1

#include <math.h>
#include <stdint.h>

typedef struct Slew {
  float start_value;
  float target_value;
  float current_value;
  float progress;
  float delta_value;
  uint32_t duration_ms;
  uint32_t last_update_time;
} Slew;

void Slew_init(Slew *slew, uint32_t duration_ms, float initial_value) {
  slew->start_value = initial_value;
  slew->target_value = initial_value;
  slew->current_value = initial_value;
  slew->progress = 0.0f;
  slew->last_update_time = 0;
  slew->duration_ms = duration_ms > 0 ? duration_ms : 1;
  slew->delta_value = 0.0f;
}

void Slew_set_target(Slew *slew, float target_value) {
  if (slew->target_value != target_value) {
    slew->start_value = slew->current_value;
    slew->target_value = target_value;
    slew->progress = 0.0f;
    slew->delta_value = target_value - slew->start_value;
    slew->last_update_time = 0;
  }
}

float Slew_process_step(Slew *slew) {
  slew->progress += 1.0f / (float)slew->duration_ms;
  if (slew->progress >= 1.0f) {
    slew->progress = 1.0f;
    return slew->target_value;
  }

  float t = slew->progress;
  float eased_t = t * t * (3.0f - 2.0f * t);  // Cubic easing function
  return slew->start_value + slew->delta_value * eased_t;
}

float Slew_process(Slew *slew, uint32_t ct) {
  if (slew->last_update_time == 0) {
    slew->last_update_time = ct;
    return slew->current_value;
  }

  uint32_t dt = ct - slew->last_update_time;
  if (dt == 0) {
    return slew->current_value;
  }

  // Run the process step for each millisecond of missing time
  for (uint32_t i = 0; i < dt; i++) {
    slew->current_value = Slew_process_step(slew);
  }

  slew->last_update_time += dt;
  return slew->current_value;
}

#endif
