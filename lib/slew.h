#ifndef SLEW_LIB
#define SLEW_LIB 1

#include <math.h>

typedef struct SlewInstance {
  float start_value;
  float target_value;
  float duration_ms;
  float progress;
  float delta_value;
  float last_update_time;
} SlewInstance;

typedef struct Slew {
  SlewInstance current_slew;
  float current_value;
} Slew;

void SlewInstance_init(SlewInstance *slew, float target_value,
                       float start_value, float duration_ms) {
  slew->start_value = start_value;
  slew->target_value = target_value;
  slew->progress = 0.0f;
  slew->last_update_time = -1.0f;
  slew->duration_ms = duration_ms > 0 ? duration_ms : 1.0f;
  slew->delta_value = target_value - start_value;
}

float SlewInstance_process(SlewInstance *slew, float ct) {
  if (slew->last_update_time < 0) {
    slew->last_update_time = ct;
    return slew->start_value;
  }

  float dt = ct - slew->last_update_time;
  if (dt < 0) {
    dt = 0;
  }

  slew->progress += dt / slew->duration_ms;
  if (slew->progress >= 1.0f) {
    slew->progress = 1.0f;
    return slew->target_value;  // Early return for completed slew
  }

  float t = slew->progress;
  // Consider using a lookup table or a more efficient easing function
  // approximation for performance-critical applications
  float eased_t = t * t * (3.0f - 2.0f * t);  // Cubic easing
  float current_value = slew->start_value + slew->delta_value * eased_t;

  slew->last_update_time = ct;
  return current_value;
}

void Slew_init(Slew *slew, float duration_ms, float initial_value) {
  SlewInstance_init(&slew->current_slew, initial_value, initial_value,
                    duration_ms);
  slew->current_value = initial_value;
}

float Slew_process(Slew *slew, float ct) {
  slew->current_value = SlewInstance_process(&slew->current_slew, ct);
  return slew->current_value;
}

void Slew_set_target(Slew *slew, float target_value) {
  if (slew->current_slew.target_value != target_value) {
    SlewInstance_init(&slew->current_slew, target_value, slew->current_value,
                      slew->current_slew.duration_ms);
  }
}

#endif