#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "../../lfo.h"

int main() {
  // Open a file to save the data
  FILE *file = fopen("wave.csv", "w");
  if (file == NULL) {
    printf("Error opening file!\n");
    return 1;
  }
  float period = 1.0f;
  Slew slew;
  Noise noise;
  Noise_init(&noise, 1234);
  Slew_init(&slew, period * 1000, 0);
  float min_voltage = 0;
  float max_voltage = 5;

  // Generate and write the triangle wave data
  for (float ct = 0; ct < 12000; ct += 10) {
    fprintf(file, "%f, %f\n", ct / 1000.0,
            get_lfo_value(4, ct, period * 1000, min_voltage, max_voltage, 0,
                          &noise, &slew));
  }

  for (float ct = 12000; ct < 12000 * 2; ct += 10) {
    fprintf(file, "%f, %f\n", ct / 1000.0,
            get_lfo_value(2, ct, period * 1000, min_voltage, max_voltage, 0,
                          &noise, &slew));
  }

  for (float ct = 12000 * 2; ct < 12500 * 2; ct += 10) {
    fprintf(file, "%f, %f\n", ct / 1000.0,
            get_lfo_value(0, ct, period * 1000, min_voltage, max_voltage, 0,
                          &noise, &slew));
  }
  for (float ct = 12500 * 2; ct < 13500 * 2; ct += 10) {
    fprintf(file, "%f, %f\n", ct / 1000.0,
            get_lfo_value(1, ct, period * 1000, min_voltage, max_voltage, 0,
                          &noise, &slew));
  }

  fclose(file);
  printf("Wave data saved to 'wave.csv'.\n");
  return 0;
}