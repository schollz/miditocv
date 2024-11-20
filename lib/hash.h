#ifndef LIB_HASH_H
#define LIB_HASH_H 1

#include <stdint.h>

uint32_t hash_djb(const char *str) {
  uint32_t hash = 5381;
  int c;
  while ((c = *str++)) {
    hash = ((hash << 5) + hash) + c;
  }
  return hash;
}

#endif