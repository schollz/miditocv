#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Type definition for the pulse callback function
typedef void (*callback_bool_int)(bool, int);

// Fractions of a beat
#define SIMPLETIMER_NUM_DIVISIONS 9

// SimpleTimer structure definition
typedef struct SimpleTimer {
  float bpm;
  float division;
  float first_time;
  float next_time;
  float offset;
  float time_diff;
  bool on;
  int user_data;
  callback_bool_int pulse_callback;
} SimpleTimer;

// Function to set BPM
void SimpleTimer_compute_time_diff(SimpleTimer *self) {
  self->time_diff = 30000.0f / (self->bpm * division);
}

void SimpleTimer_set_bpm(SimpleTimer *self, float bpm) {
  self->bpm = bpm;

  // compute new time diff
  SimpleTimer_compute_time_diff(self);

  // compute new next time
  float current_next_time = self->next_time;
  if (current_next_time > 0) {
    self->next_time = self->first_time + self->offset + self->time_diff;
    self->on = true;
    while (self->next_time < current_next_time) {
      self->next_time += self->time_diff;
      self->on = !self->on;
    }
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
                      int user_data) {
  self->bpm = bpm;
  self->on = false;
  self->offset = offset;
  self->pulse_callback = pulse_callback;
  self->next_time = 0;
  self->user_data = user_data;
  SimpleTimer_set_division(self, division);
}

// Function to reset the SimpleTimer state
void SimpleTimer_reset(SimpleTimer *self, float current_time) {
  self->on = true;
  self->next_time = current_time + self->time_diff + self->offset;
  if (self->pulse_callback != NULL) {
    self->pulse_callback(self->on, self->user_data);
  }
}

// Function to stop the SimpleTimer
void SimpleTimer_stop(SimpleTimer *self) { self->next_time = 0; }

// Function to start the SimpleTimer
void SimpleTimer_start(SimpleTimer *self, float current_time) {
  SimpleTimer_reset(self, current_time);
}

// Main processing function for generating timer pulses
bool SimpleTimer_process(SimpleTimer *self, float current_time) {
  if (self->next_time == 0) {
    return false;
  }

  // Calculate the time difference, considering possible overflow
  if (current_time >= self->next_time) {
    self->on = !self->on;
    self->next_time += self->time_diff;
    if (self->pulse_callback != NULL) {
      self->pulse_callback(self->on, self->user_data);
    }
    return self->on;
  }
  return false;
}

#endif
