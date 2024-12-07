#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "../../adsr.h"

int main() {
  // Define the ADSR parameters: attack, decay, sustain, release, shape
  ADSR adsr;
  ADSR_init(&adsr, 1000.0f, 2000.0f, 0.5f, 2000.0f, 5.0f);

  // Open the output CSV file
  FILE *file = fopen("adsr_output.csv", "w");
  if (!file) {
    perror("Error opening file");
    return 1;
  }

  // Write the CSV header
  fprintf(file, "time_ms,level,state\n");

  // Simulation parameters
  uint32_t current_time = 0;
  uint32_t total_time = 10000;  // Total simulation time in milliseconds

  // Start with the gate off
  ADSR_gate(&adsr, false, current_time);

  // Run the simulation
  uint32_t time_increment = 2;
  for (current_time = 0; current_time <= total_time;
       current_time += time_increment) {
    float val = ADSR_process(&adsr, current_time);
    fprintf(file, "%u,%f,%d\n", current_time, val, adsr.state);
    if (current_time == 500) {
      ADSR_gate(&adsr, true, current_time);
    }
    if (current_time == 4500) {
      ADSR_gate(&adsr, false, current_time);
    }
    if (current_time == 5700) {
      ADSR_gate(&adsr, true, current_time);
    }
    if (current_time == 8000) {
      ADSR_gate(&adsr, false, current_time);
    }
  }

  fclose(file);
  printf("ADSR simulation complete. Output saved to adsr_output.csv.\n");

  return 0;
}