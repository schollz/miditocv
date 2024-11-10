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

  LFO lfo;
  LFO_init(&lfo, LFO_DRUNK, 1000, 0, 100, 0);

  // Generate and write the triangle wave data
  for (float ct = 0; ct < 12000; ct += 1) {
    float value = LFO_update(&lfo, ct);
    fprintf(file, "%f, %f\n", ct / 1000.0, value);
    if (ct == 3000) {
      LFO_set_type(&lfo, LFO_SINE, ct);
    }
    if (ct == 6000) {
      LFO_set_type(&lfo, LFO_SAWTOOTH, ct);
    }
  }

  fclose(file);
  printf("Wave data saved to 'wave.csv'.\n");
  return 0;
}