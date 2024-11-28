#include <math.h>
#include <stdio.h>

#include "../../lfo.h"

int main() {
  // Open a file to save the data
  FILE *file = fopen("wave.csv", "w");
  if (file == NULL) {
    printf("Error opening file!\n");
    return 1;
  }
  float period = 100.0f;
  float min_voltage = 0.0f;
  float max_voltage = 10.0f;
  float phase = 0.0f;
  Noise noise;
  Noise_init(&noise, 0);

  // Generate and write the triangle wave data
  for (float ct = 0; ct < 12000; ct += 10) {
    float value = get_lfo_value(LFO_DRUNK, ct, period * 1000, min_voltage,
                                max_voltage, phase, &noise);

    fprintf(file, "%f, %f\n", ct / 1000.0, value);
  }

  fclose(file);
  printf("Wave data saved to 'wave.csv'.\n");
  return 0;
}