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
  uint32_t next_pulse_difference;
  uint32_t last_pulse_time;
  bool on;
  callback_bool pulse_callback;
} Clockout;

// Function to set BPM
void Clockout_set_bpm(Clockout *self, float bpm) { self->bpm = bpm; }

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
  self->next_pulse_difference = 0;
  Clockout_set_division(self, division);
}

// Function to calculate the next pulse difference
void Clockout_set_next_pulse_difference(Clockout *self, int32_t error) {
  self->next_pulse_difference =
      (uint32_t)(roundf(30000.0f /
                        (self->bpm * self_divisions[self->division])) -
                 error);
}

// Function to reset the Clockout state
void Clockout_reset(Clockout *self, uint32_t current_time) {
  self->on = false;
  self->last_pulse_time = current_time;
  Clockout_set_next_pulse_difference(self, 0);
}

// Function to stop the Clockout
void Clockout_stop(Clockout *self) { self->next_pulse_difference = 0; }

// Function to start the Clockout
void Clockout_start(Clockout *self, uint32_t current_time) {
  Clockout_reset(self, current_time);
}

// Main processing function for generating clock pulses
void Clockout_process(Clockout *self, uint32_t current_time) {
  if (self->next_pulse_difference == 0) {
    return;
  }

  // Calculate the time difference, considering possible overflow
  uint32_t time_diff = current_time - self->last_pulse_time;

  if (time_diff >= self->next_pulse_difference) {
    self->on = !self->on;
    int32_t error = (int32_t)(time_diff - self->next_pulse_difference);
    printf("Error: %d\n", error);
    Clockout_set_next_pulse_difference(self, error);
    if (self->pulse_callback != NULL) {
      self->pulse_callback(self->on);
    }
    self->last_pulse_time = current_time;
  }
}

#endif
