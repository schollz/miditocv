#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../spiral.h"

// Test the function
int main() {
  int input_value = 512;  // Example input
  float x, y;
  for (uint16_t i = 0; i < 1023; i++) {
    spiral_coordinate(i, &x, &y);
    printf("%f %f\n", x, y);
  }

  return 0;
}