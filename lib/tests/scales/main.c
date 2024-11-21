#include <stdint.h>
#include <stdio.h>

#include "../../scales.h"

int main() {
  float voltage;
  voltage = 1.5f / 12.0f;
  printf("closest: %2.3f->%2.3f\n", voltage,
         scale_quantize_voltage(0, voltage));
  return 0;
}