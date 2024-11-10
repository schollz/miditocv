#ifndef SLEW_LIB
#define SLEW_LIB 1

typedef struct Slew {
  float current_value;
  float target_value;
  float start_value;
  float duration;          // Duration of the transition in milliseconds
  float progress;          // Progress of the transition (0 to 1)
  float last_update_time;  // Time of the last update
} Slew;

// Set the slew duration based on a period in milliseconds
void Slew_set_period(Slew *slew, float period_ms) {
  if (period_ms <= 0) {
    slew->duration = 0;
  } else {
    slew->duration = period_ms;
  }
}

// Initialize the Slew object
void Slew_init(Slew *slew, float slew_period, float initial_value) {
  slew->current_value = initial_value;
  slew->target_value = initial_value;
  slew->start_value = initial_value;
  slew->progress = 0.0f;
  slew->last_update_time = -1.0f;  // Use -1 to indicate uninitialized time
  Slew_set_period(slew, slew_period);
}

// Update the current value based on the elapsed time (quadratic smoothing)
float Slew_process(Slew *slew, float ct) {
  if (slew->last_update_time < 0 || slew->duration == 0) {
    slew->last_update_time = ct;
    return slew->current_value;
  }

  float delta_time = ct - slew->last_update_time;

  // Ensure delta_time is non-negative
  if (delta_time < 0) {
    delta_time = 0;
  }

  // Update progress based on elapsed time
  slew->progress += delta_time / slew->duration;

  // Clamp progress to the range [0, 1]
  if (slew->progress > 1.0f) {
    slew->progress = 1.0f;
  }

  // Quadratic easing in and out (smooth transition)
  float t = slew->progress;
  float eased_t = t < 0.5f ? 2 * t * t : 1 - (-2 * t + 2) * (-2 * t + 2) / 2;

  // Update the current value based on eased progress
  slew->current_value =
      slew->start_value + (slew->target_value - slew->start_value) * eased_t;

  // If the transition is complete, reset progress
  if (slew->progress >= 1.0f) {
    slew->current_value = slew->target_value;
    slew->progress = 0.0f;
    slew->start_value = slew->target_value;
  }

  slew->last_update_time = ct;
  return slew->current_value;
}

// Set a new target value and reset the transition
void Slew_set_target(Slew *slew, float target_value) {
  if (slew->target_value != target_value) {
    slew->start_value = slew->current_value;
    slew->target_value = target_value;
    slew->progress = 0.0f;
  }
}

#endif
