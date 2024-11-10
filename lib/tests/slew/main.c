#include <math.h>
#include <stdint.h>
#include <stdio.h>

#include "../../slew.h"

int main() {
  // Open a file to save the data
  FILE *file = fopen("wave.csv", "w");
  if (file == NULL) {
    printf("Error opening file!\n");
    return 1;
  }

  Slew slew;
  Slew_init(&slew, 3000, 0);

  // Generate and write the triangle wave data
  uint32_t dt = 1;
  for (uint32_t ct = 0; ct < 12000; ct += dt) {
    if (ct == 1000) {
      dt = 1;
      Slew_set_target(&slew, 3.14);
    } else if (ct == 2000) {
      Slew_set_target(&slew, -5);
    } else if (ct == 6000) {
      Slew_set_target(&slew, 6.28);
    } else if (ct == 6000) {
      dt = 40;
      Slew_set_target(&slew, 0);
    }
    float value = Slew_process(&slew, ct);
    fprintf(file, "%f,%f\n", (float)ct / 1000.0f, value);
  }

  fclose(file);
  printf("Wave data saved to 'wave.csv'.\n");
  return 0;
}