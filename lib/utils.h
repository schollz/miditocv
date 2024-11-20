#ifndef LIB_UTILS_H
#define LIB_UTILS_H 1

#include <stdint.h>
#include <stdio.h>

int parse_wxyz(const char *input, int *W, int *X, uint32_t *Y, float *Z) {
  if (!input || !W || !X || !Y) {
    return 0;  // Return 0 if required pointers are invalid
  }

  if (Z) {
    // If Z is provided, attempt to parse all four fields
    return sscanf(input, "%d_%d_%u_%f", W, X, Y, Z);
  } else {
    // If Z is NULL, parse only the first three fields
    return sscanf(input, "%d_%d_%u", W, X, Y);
  }
}

#endif