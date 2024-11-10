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
  Slew_init(&slew, 2000, 0);

  // Generate and write the triangle wave data
  for (float ct = 0; ct < 12000; ct += 1) {
    if (ct == 1000) {
      Slew_set_target(&slew, 1);
    } else if (ct == 1501) {
      Slew_set_target(&slew, 10);
    } else if (ct == 5000) {
      Slew_set_target(&slew, 0.45);
    } else if (ct == 7000) {
      Slew_set_target(&slew, 11.34);
    } else if (ct == 9000) {
      Slew_set_target(&slew, 5.3);
    } else if (ct == 11000) {
      Slew_set_target(&slew, 4.0);
    }
    float value = Slew_process(&slew, ct);
    fprintf(file, "%f,%f\n", ct, value);
  }

  fclose(file);
  printf("Wave data saved to 'wave.csv'.\n");
  return 0;
}