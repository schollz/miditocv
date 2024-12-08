#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Type definition for the pulse callback function
typedef void (*callback_bool_int)(bool, int);

// SimpleTimer structure definition
typedef struct SimpleTimer {
  float bpm;
  float division;
  float first_time;
  float next_time;
  float last_time;
  float offset;
  float time_diff;
  int user_data;
  bool on;
  callback_bool_int pulse_callback;
} SimpleTimer;

void SimpleTimer_set_bpm(SimpleTimer *self, float bpm) {
  self->bpm = bpm;

  // compute new time diff
  self->time_diff = 30000.0f / (self->bpm * self->division);

  // compute new next time
  self->next_time = self->first_time + self->offset + self->time_diff;
  self->on = true;
  while (self->next_time < self->last_time) {
    self->next_time += self->time_diff;
    self->on = !self->on;
  }
}

// Function to set the division, ensuring it is within valid range
void SimpleTimer_set_division(SimpleTimer *self, float division) {
  self->division = division;
  // recompute
  SimpleTimer_set_bpm(self, self->bpm);
}

// Initialization function for SimpleTimer
void SimpleTimer_init(SimpleTimer *self, float bpm, float division,
                      float offset, callback_bool_int pulse_callback,
                      int user_data, float current_time) {
  self->offset = offset;
  self->pulse_callback = pulse_callback;
  self->user_data = user_data;
  self->first_time = current_time;
  self->last_time = current_time;
  self->bpm = bpm;
  SimpleTimer_set_division(self, division);
}

// Main processing function for generating timer pulses
bool SimpleTimer_process(SimpleTimer *self, float current_time) {
  self->last_time = current_time;
  if (current_time < self->next_time) {
    return false;
  }
  // Calculate the time difference, considering possible overflow
  if (current_time >= self->next_time) {
    while (self->next_time < current_time) {
      self->on = !self->on;
      self->next_time += self->time_diff;
    }
    if (self->pulse_callback != NULL) {
      self->pulse_callback(self->on, self->user_data);
    }
    return self->on;
  }
  return false;
}

#endif
