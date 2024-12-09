#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../simpletimer.h"

#define DURATION_MS 8000
#define TIMESTEP_MS 1
float current_time = 0.0f;
FILE *file;

// Callback function for Timer 1
void timer1_callback(bool on, int user_data) {
  fprintf(file, "%2.1f,1,%d\n", current_time, on ? 1 : 0);
}

// Callback function for Timer 2
void timer2_callback(bool on, int user_data) {
  fprintf(file, "%2.1f,2,%d\n", current_time, on ? 1 : 0);
}

void timer3_callback(bool on, int user_data) {
  fprintf(file, "%2.1f,3,%d\n", current_time, on ? 1 : 0);
}

int main() {
  // Open a file to save the output
  file = fopen("simple_timer_output.csv", "w");
  if (file == NULL) {
    printf("Error opening file!\n");
    return 1;
  }

  // Initialize two timers
  SimpleTimer timer1, timer2, timer3;
  SimpleTimer_init(&timer1, 237.0f, 1, 0.0, timer1_callback, 0,
                   0);  // 120 BPM, quarter note
  SimpleTimer_init(&timer2, 90.0f, 1, 0, timer2_callback, 1,
                   0);  // 90 BPM, half note
  SimpleTimer_init(&timer3, 190.0f, 1, 0.0, timer3_callback, 2, 0);
  SimpleTimer_start(&timer2);
  SimpleTimer_start(&timer3);

  for (current_time = 0; current_time <= DURATION_MS;
       current_time += TIMESTEP_MS) {
    SimpleTimer_process(&timer1, current_time);
    SimpleTimer_process(&timer2, current_time);
    SimpleTimer_process(&timer3, current_time);

    if (current_time == 2000) {
      // set the bpm of timer1 to 90
      SimpleTimer_set_bpm(&timer1, 90.0f);
    }
    if (current_time == 4000) {
      // set the bpm of timer2 to 190
      SimpleTimer_set_bpm(&timer1, 190.0f);
      SimpleTimer_set_bpm(&timer2, 190.0f);
      SimpleTimer_stop(&timer3);
    }
    if (current_time == 5000) {
      SimpleTimer_start(&timer3);
    }
    if (current_time == 6000) {
      SimpleTimer_set_bpm(&timer1, 60.0f);
      SimpleTimer_set_bpm(&timer2, 60.0f);
      SimpleTimer_set_bpm(&timer3, 60.0f);
    }
    if (current_time == 3000) {
      SimpleTimer_reset(&timer1, current_time);
      SimpleTimer_start(&timer1);
    }
    // Increment time step
    current_time += TIMESTEP_MS;
  }

  // Close the file
  fclose(file);

  printf("Simulation complete. Data saved to simple_timer_output.csv\n");
  return 0;
}