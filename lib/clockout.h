#ifndef CLOCKOUT_H
#define CLOCKOUT_H

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// Type definition for the pulse callback function
typedef void (*callback_bool)(bool);

// Fractions of a beat
#define CLOCKOUT_NUM_DIVISIONS 9
float self_divisions[CLOCKOUT_NUM_DIVISIONS] = {
    1.0 / 16, 1.0 / 8, 1.0 / 4, 1.0 / 2, 1.0, 2.0, 4.0, 8.0, 16.0};

// Clockout structure definition
typedef struct Clockout {
  float bpm;
  uint8_t division;
  float next_time;
  float time_diff;
  bool on;
  callback_bool pulse_callback;
} Clockout;

// Function to set BPM
void Clockout_set_bpm(Clockout *self, float bpm) { self->bpm = bpm; }

void Clockout_compute_time_diff(Clockout *self) {
  self->time_diff = 30000.0f / (self->bpm * self_divisions[self->division]);
}

// Function to set the division, ensuring it is within valid range
void Clockout_set_division(Clockout *self, uint8_t division) {
  if (division < CLOCKOUT_NUM_DIVISIONS) {
    self->division = division;
  }
}

// Initialization function for Clockout
void Clockout_init(Clockout *self, float bpm, uint8_t division,
                   callback_bool pulse_callback) {
  self->bpm = bpm;
  self->division = 0;
  self->on = false;
  self->pulse_callback = pulse_callback;
  self->next_time = 0;
  Clockout_set_division(self, division);
  Clockout_compute_time_diff(self);
}

// Function to reset the Clockout state
void Clockout_reset(Clockout *self, float current_time) {
  self->on = true;
  self->next_time = current_time + self->time_diff;
  if (self->pulse_callback != NULL) {
    self->pulse_callback(self->on);
  }
}

// Function to stop the Clockout
void Clockout_stop(Clockout *self) { self->next_time = 0; }

// Function to start the Clockout
void Clockout_start(Clockout *self, float current_time) {
  Clockout_reset(self, current_time);
}

// Main processing function for generating clock pulses
void Clockout_process(Clockout *self, float current_time) {
  if (self->next_time == 0) {
    return;
  }

  // Calculate the time difference, considering possible overflow
  if (current_time >= self->next_time) {
    self->on = !self->on;
    self->next_time += self->time_diff;
    if (self->pulse_callback != NULL) {
      self->pulse_callback(self->on);
    }
  }
}

#endif
