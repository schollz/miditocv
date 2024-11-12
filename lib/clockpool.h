#ifndef LIBCLOCKPOOL_H
#define LIBCLOCKPOOL_H

#include <stdbool.h>
#include <stdio.h>

#include "pico/multicore.h"
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

// Define distinct ClockState variables for each clock
ClockState clockpool_state0, clockpool_state1, clockpool_state2,
    clockpool_state3;
ClockState clockpool_state4, clockpool_state5, clockpool_state6,
    clockpool_state7;

// Define distinct timers for each clock
struct repeating_timer timer0, timer1, timer2, timer3, timer4, timer5, timer6,
    timer7;

// Helper function to initialize a clock state
void init_clock_state(ClockState *state) {
  state->step = -1;
  state->enabled = false;
  state->division = 1.0;
  state->bpm = 120.0;
  state->min_voltage = 0.0;
  state->max_voltage = 3.3;
}

// Helper function to handle timer updates
void update_clock_state(ClockState *state, int index) {
  if (state->enabled) {
    state->step++;
    state->on = !state->on;
    // TODO: Change voltage here
    printf("[%d] %lld %d %d\n", index, time_us_64(), state->step, state->on);
  }
}

// Timer callback functions for each clock
bool repeating_timer_callback0(struct repeating_timer *t) {
  update_clock_state(&clockpool_state0, 0);
  return true;
}
bool repeating_timer_callback1(struct repeating_timer *t) {
  update_clock_state(&clockpool_state1, 1);
  return true;
}
bool repeating_timer_callback2(struct repeating_timer *t) {
  update_clock_state(&clockpool_state2, 2);
  return true;
}
bool repeating_timer_callback3(struct repeating_timer *t) {
  update_clock_state(&clockpool_state3, 3);
  return true;
}
bool repeating_timer_callback4(struct repeating_timer *t) {
  update_clock_state(&clockpool_state4, 4);
  return true;
}
bool repeating_timer_callback5(struct repeating_timer *t) {
  update_clock_state(&clockpool_state5, 5);
  return true;
}
bool repeating_timer_callback6(struct repeating_timer *t) {
  update_clock_state(&clockpool_state6, 6);
  return true;
}
bool repeating_timer_callback7(struct repeating_timer *t) {
  update_clock_state(&clockpool_state7, 7);
  return true;
}

void ClockPool_init_() {
  // init_clock_state(&clockpool_state0);
  //  init_clock_state(&clockpool_state1);
  //  init_clock_state(&clockpool_state2);
  //  init_clock_state(&clockpool_state3);
  //  init_clock_state(&clockpool_state4);
  //  init_clock_state(&clockpool_state5);
  //  init_clock_state(&clockpool_state6);
  //  init_clock_state(&clockpool_state7);

  // add_repeating_timer_ms(1000, repeating_timer_callback0, NULL, &timer0);
  //  add_repeating_timer_ms(1000, repeating_timer_callback1, NULL, &timer1);
  //  add_repeating_timer_ms(1000, repeating_timer_callback2, NULL, &timer2);
  //  add_repeating_timer_ms(1000, repeating_timer_callback3, NULL, &timer3);
  //  add_repeating_timer_ms(1000, repeating_timer_callback4, NULL, &timer4);
  //  add_repeating_timer_ms(1000, repeating_timer_callback5, NULL, &timer5);
  //  add_repeating_timer_ms(1000, repeating_timer_callback6, NULL, &timer6);
  //  add_repeating_timer_ms(1000, repeating_timer_callback7, NULL, &timer7);

  // Core 1 main loop
  while (1) {
    // Perform any additional tasks needed on core 1
    tight_loop_contents();  // Prevent the core from idling
  }
}

void ClockPool_init() { multicore_launch_core1(ClockPool_init_); }

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
    default:
      printf("Invalid index: %d\n", index);
      break;
  }
}

// Reset a specific clock
void ClockPool_reset_clock(int index, float bpm, float division,
                           float min_voltage, float max_voltage) {
  ClockState *state;
  struct repeating_timer *timer;
  bool (*callback)(struct repeating_timer *);

  switch (index) {
    case 0:
      state = &clockpool_state0;
      timer = &timer0;
      callback = repeating_timer_callback0;
      break;
    case 1:
      state = &clockpool_state1;
      timer = &timer1;
      callback = repeating_timer_callback1;
      break;
    case 2:
      state = &clockpool_state2;
      timer = &timer2;
      callback = repeating_timer_callback2;
      break;
    case 3:
      state = &clockpool_state3;
      timer = &timer3;
      callback = repeating_timer_callback3;
      break;
    case 4:
      state = &clockpool_state4;
      timer = &timer4;
      callback = repeating_timer_callback4;
      break;
    case 5:
      state = &clockpool_state5;
      timer = &timer5;
      callback = repeating_timer_callback5;
      break;
    case 6:
      state = &clockpool_state6;
      timer = &timer6;
      callback = repeating_timer_callback6;
      break;
    case 7:
      state = &clockpool_state7;
      timer = &timer7;
      callback = repeating_timer_callback7;
      break;
    default:
      printf("Invalid index: %d\n", index);
      return;
  }

  cancel_repeating_timer(timer);
  state->step = -1;
  state->division = division;
  state->bpm = bpm;
  state->min_voltage = min_voltage;
  state->max_voltage = max_voltage;
  state->on = false;

  update_clock_state(state, index);
  int interval_ms = roundf((30000.0f / bpm) / division);
  add_repeating_timer_ms(interval_ms, callback, NULL, timer);
}

#endif
