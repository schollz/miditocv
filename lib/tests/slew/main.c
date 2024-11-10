#include <math.h>
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
  for (float ct = 0; ct < 12000; ct += 1) {
    if (ct < 4000) {
      Slew_set_target(&slew, 7.0);
    } else if (ct < 5000) {
      Slew_set_target(&slew, 3.2);
    } else {
      Slew_set_target(&slew, 1.7);
    }
    float value = Slew_process(&slew, ct);
    fprintf(file, "%f,%f\n", ct, value);
  }

  fclose(file);
  printf("Wave data saved to 'wave.csv'.\n");
  return 0;
}