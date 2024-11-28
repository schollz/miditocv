// Copyright 2023-2024 Zack Scholl.
// Simplified for LFNoise2 only.

#ifndef NOISE_LIB
#define NOISE_LIB 1

#include <stdint.h>
#include <stdlib.h>

typedef struct Noise {
  uint32_t s1, s2, s3;  // Random generator state
} Noise;

// Helper function declarations
uint32_t trand(Noise *noise);
float frand2(Noise *noise);
void fseed(Noise *noise, uint32_t seed);

// Helper functions

uint32_t trand(Noise *noise) {
  noise->s1 = ((noise->s1 & (uint32_t)-2) << 12) ^
              (((noise->s1 << 13) ^ noise->s1) >> 19);
  noise->s2 = ((noise->s2 & (uint32_t)-8) << 4) ^
              (((noise->s2 << 2) ^ noise->s2) >> 25);
  noise->s3 = ((noise->s3 & (uint32_t)-16) << 17) ^
              (((noise->s3 << 3) ^ noise->s3) >> 11);
  return noise->s1 ^ noise->s2 ^ noise->s3;
}

float frand2(Noise *noise) {
  union {
    uint32_t i;
    float f;
  } u;
  u.i = 0x40000000 | (trand(noise) >> 9);
  return u.f - 3.f;
}

void fseed(Noise *noise, uint32_t seed) {
  noise->s1 = 1243598713U ^ seed;
  if (noise->s1 < 2) noise->s1 = 1243598713U;
  noise->s2 = 3093459404U ^ seed;
  if (noise->s2 < 8) noise->s2 = 3093459404U;
  noise->s3 = 1821928721U ^ seed;
  if (noise->s3 < 16) noise->s3 = 1821928721U;
}

void Noise_init(Noise *noise, uint32_t seed) { fseed(noise, seed); }

#endif /* NOISE_LIB */
