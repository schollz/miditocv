#ifndef LIBCLOCKPOOL_H
#define LIBCLOCKPOOL_H

#include <stdbool.h>
#include <stdio.h>

#include "pico/stdlib.h"

// Define a struct to hold the step and a change flag
typedef struct {
  int step;
  bool on;
  bool enabled;
  float division;
  float bpm;
  float min_voltage;
  float max_voltage;
} ClockState;

// Global variables for the state of each timer
ClockState clockpool_state0 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state1 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state2 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state3 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state4 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state5 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state6 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};
ClockState clockpool_state7 = {.step = -1,

                               .enabled = false,
                               .division = 1.0,
                               .bpm = 120.0,
                               .min_voltage = 0.0,
                               .max_voltage = 3.3};

struct repeating_timer timer0;
struct repeating_timer timer1;
struct repeating_timer timer2;
struct repeating_timer timer3;
struct repeating_timer timer4;
struct repeating_timer timer5;
struct repeating_timer timer6;
struct repeating_timer timer7;

// Callback functions for each timer
bool repeating_timer_callback0(struct repeating_timer *t) {
  if (clockpool_state0.enabled) {
    clockpool_state0.step++;
    clockpool_state0.on = !clockpool_state0.on;
    // TODO change voltage here
    printf("[0] %lld %d %d\n", time_us_64(), clockpool_state0.step,
           clockpool_state0.on);
  }
  return true;
}

bool repeating_timer_callback1(struct repeating_timer *t) {
  if (clockpool_state1.enabled) {
    clockpool_state1.step++;
    clockpool_state1.on = !clockpool_state1.on;
    // TODO change voltage here
    printf("[1] %lld %d %d\n", time_us_64(), clockpool_state1.step,
           clockpool_state1.on);
  }
  return true;
}

bool repeating_timer_callback2(struct repeating_timer *t) {
  if (clockpool_state2.enabled) {
    clockpool_state2.step++;
    clockpool_state2.on = !clockpool_state2.on;
    // TODO change voltage here
    printf("[2] %lld %d %d\n", time_us_64(), clockpool_state2.step,
           clockpool_state2.on);
  }
  return true;
}

bool repeating_timer_callback3(struct repeating_timer *t) {
  if (clockpool_state3.enabled) {
    clockpool_state3.step++;
    clockpool_state3.on = !clockpool_state3.on;
    // TODO change voltage here
    printf("[3] %lld %d %d\n", time_us_64(), clockpool_state3.step,
           clockpool_state3.on);
  }
  return true;
}

bool repeating_timer_callback4(struct repeating_timer *t) {
  if (clockpool_state4.enabled) {
    clockpool_state4.step++;
    clockpool_state4.on = !clockpool_state4.on;
    // TODO change voltage here
    printf("[4] %lld %d %d\n", time_us_64(), clockpool_state4.step,
           clockpool_state4.on);
  }
  return true;
}

bool repeating_timer_callback5(struct repeating_timer *t) {
  if (clockpool_state5.enabled) {
    clockpool_state5.step++;
    clockpool_state5.on = !clockpool_state5.on;
    // TODO change voltage here
    printf("[5] %lld %d %d\n", time_us_64(), clockpool_state5.step,
           clockpool_state5.on);
  }
  return true;
}

bool repeating_timer_callback6(struct repeating_timer *t) {
  if (clockpool_state6.enabled) {
    clockpool_state6.step++;
    clockpool_state6.on = !clockpool_state6.on;
    // TODO change voltage here
    printf("[6] %lld %d %d\n", time_us_64(), clockpool_state6.step,
           clockpool_state6.on);
  }
  return true;
}

bool repeating_timer_callback7(struct repeating_timer *t) {
  if (clockpool_state7.enabled) {
    clockpool_state7.step++;
    clockpool_state7.on = !clockpool_state7.on;
    // TODO change voltage here
    printf("[7] %lld %d %d\n", time_us_64(), clockpool_state7.step,
           clockpool_state7.on);
  }
  return true;
}

void ClockPool_init() {
  add_repeating_timer_ms(1000, repeating_timer_callback0, NULL, &timer0);
  add_repeating_timer_ms(1000, repeating_timer_callback1, NULL, &timer1);
  add_repeating_timer_ms(1000, repeating_timer_callback2, NULL, &timer2);
  add_repeating_timer_ms(1000, repeating_timer_callback3, NULL, &timer3);
  add_repeating_timer_ms(1000, repeating_timer_callback4, NULL, &timer4);
  add_repeating_timer_ms(1000, repeating_timer_callback5, NULL, &timer5);
  add_repeating_timer_ms(1000, repeating_timer_callback6, NULL, &timer6);
  add_repeating_timer_ms(1000, repeating_timer_callback7, NULL, &timer7);
}

void ClockPool_enable(int index, bool enable) {
  switch (index) {
    case 0:
      clockpool_state0.enabled = enable;
      break;
    case 1:
      clockpool_state1.enabled = enable;
      break;
    case 2:
      clockpool_state2.enabled = enable;
      break;
    case 3:
      clockpool_state3.enabled = enable;
      break;
    case 4:
      clockpool_state4.enabled = enable;
      break;
    case 5:
      clockpool_state5.enabled = enable;
      break;
    case 6:
      clockpool_state6.enabled = enable;
      break;
    case 7:
      clockpool_state7.enabled = enable;
      break;
  }
}

// Reset a specific clock
void ClockPool_reset_clock(int index, float bpm, float division,
                           float min_voltage, float max_voltage) {
  switch (index) {
    case 0:
      cancel_repeating_timer(&timer0);
      clockpool_state0.step = 0;
      clockpool_state0.division = division;
      clockpool_state0.bpm = bpm;
      clockpool_state0.min_voltage = min_voltage;
      clockpool_state0.max_voltage = max_voltage;
      clockpool_state0.on = true;
      if (clockpool_state0.enabled) {
        // TODO: Change voltage here
        printf("[0] %lld %d %d\n", time_us_64(), clockpool_state0.step,
               clockpool_state0.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state0.bpm) / clockpool_state0.division),
          repeating_timer_callback0, NULL, &timer0);
      break;

    case 1:
      cancel_repeating_timer(&timer1);
      clockpool_state1.step = 0;
      clockpool_state1.division = division;
      clockpool_state1.bpm = bpm;
      clockpool_state1.min_voltage = min_voltage;
      clockpool_state1.max_voltage = max_voltage;
      clockpool_state1.on = true;
      if (clockpool_state1.enabled) {
        printf("[1] %lld %d %d\n", time_us_64(), clockpool_state1.step,
               clockpool_state1.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state1.bpm) / clockpool_state1.division),
          repeating_timer_callback1, NULL, &timer1);
      break;

    case 2:
      cancel_repeating_timer(&timer2);
      clockpool_state2.step = 0;
      clockpool_state2.division = division;
      clockpool_state2.bpm = bpm;
      clockpool_state2.min_voltage = min_voltage;
      clockpool_state2.max_voltage = max_voltage;
      clockpool_state2.on = true;
      if (clockpool_state2.enabled) {
        printf("[2] %lld %d %d\n", time_us_64(), clockpool_state2.step,
               clockpool_state2.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state2.bpm) / clockpool_state2.division),
          repeating_timer_callback2, NULL, &timer2);
      break;

    case 3:
      cancel_repeating_timer(&timer3);
      clockpool_state3.step = 0;
      clockpool_state3.division = division;
      clockpool_state3.bpm = bpm;
      clockpool_state3.min_voltage = min_voltage;
      clockpool_state3.max_voltage = max_voltage;
      clockpool_state3.on = true;
      if (clockpool_state3.enabled) {
        printf("[3] %lld %d %d\n", time_us_64(), clockpool_state3.step,
               clockpool_state3.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state3.bpm) / clockpool_state3.division),
          repeating_timer_callback3, NULL, &timer3);
      break;

    case 4:
      cancel_repeating_timer(&timer4);
      clockpool_state4.step = 0;
      clockpool_state4.division = division;
      clockpool_state4.bpm = bpm;
      clockpool_state4.min_voltage = min_voltage;
      clockpool_state4.max_voltage = max_voltage;
      clockpool_state4.on = true;
      if (clockpool_state4.enabled) {
        printf("[4] %lld %d %d\n", time_us_64(), clockpool_state4.step,
               clockpool_state4.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state4.bpm) / clockpool_state4.division),
          repeating_timer_callback4, NULL, &timer4);
      break;

    case 5:
      cancel_repeating_timer(&timer5);
      clockpool_state5.step = 0;
      clockpool_state5.division = division;
      clockpool_state5.bpm = bpm;
      clockpool_state5.min_voltage = min_voltage;
      clockpool_state5.max_voltage = max_voltage;
      clockpool_state5.on = true;
      if (clockpool_state5.enabled) {
        printf("[5] %lld %d %d\n", time_us_64(), clockpool_state5.step,
               clockpool_state5.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state5.bpm) / clockpool_state5.division),
          repeating_timer_callback5, NULL, &timer5);
      break;

    case 6:
      cancel_repeating_timer(&timer6);
      clockpool_state6.step = 0;
      clockpool_state6.division = division;
      clockpool_state6.bpm = bpm;
      clockpool_state6.min_voltage = min_voltage;
      clockpool_state6.max_voltage = max_voltage;
      clockpool_state6.on = true;
      if (clockpool_state6.enabled) {
        printf("[6] %lld %d %d\n", time_us_64(), clockpool_state6.step,
               clockpool_state6.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state6.bpm) / clockpool_state6.division),
          repeating_timer_callback6, NULL, &timer6);
      break;

    case 7:
      cancel_repeating_timer(&timer7);
      clockpool_state7.step = 0;
      clockpool_state7.division = division;
      clockpool_state7.bpm = bpm;
      clockpool_state7.min_voltage = min_voltage;
      clockpool_state7.max_voltage = max_voltage;
      clockpool_state7.on = true;
      if (clockpool_state7.enabled) {
        printf("[7] %lld %d %d\n", time_us_64(), clockpool_state7.step,
               clockpool_state7.on);
      }
      add_repeating_timer_ms(
          roundf((30000.0f / clockpool_state7.bpm) / clockpool_state7.division),
          repeating_timer_callback7, NULL, &timer7);
      break;

    default:
      printf("Invalid index: %d\n", index);
      break;
  }
}

#endif
