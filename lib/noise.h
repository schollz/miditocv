// Copyright 2023-2024 Zack Scholl.
// Simplified for LFNoise2 only.

#ifndef NOISE_LIB
#define NOISE_LIB 1

#include <stdint.h>
#include <stdlib.h>

typedef struct Noise {
  float m_nextmidpt;
  float m_nextvalue;
  float level;
  float curve;
  float slope;
  uint32_t s1, s2, s3;     // Random generator state
  float last_update_time;  // Last update time as a float (in milliseconds)
} Noise;

// Helper function declarations
uint32_t trand(Noise *noise);
float frand2(Noise *noise);
void fseed(Noise *noise, uint32_t seed);

// Create and initialize a Noise object
Noise *Noise_create(uint32_t seed);
float LFNoise2(Noise *noise, float current_time, float period, float min_val,
               float max_val);
void Noise_destroy(Noise *noise);

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

void Noise_init(Noise *noise, uint32_t seed) {
  noise->m_nextmidpt = 0;
  noise->m_nextvalue = 0;
  noise->level = 0;
  noise->curve = 0;
  noise->slope = 0;
  noise->last_update_time = 0.0f;
  fseed(noise, seed);
}

float LFNoise2(Noise *noise, float current_time, float period, float min_val,
               float max_val) {
  float interval = 1000.0f / period;  // Interval in milliseconds

  // Check if it's time to update the noise value
  if (current_time - noise->last_update_time >= interval) {
    float value = noise->m_nextvalue;
    noise->m_nextvalue = frand2(noise);
    noise->level = noise->m_nextmidpt;
    noise->m_nextmidpt = (noise->m_nextvalue + value) * 0.5;
    noise->last_update_time = current_time;

    float fseglen = interval;
    noise->curve =
        2.0 * (noise->m_nextmidpt - noise->level - fseglen * noise->slope) /
        (fseglen * fseglen + fseglen);
  }

  noise->slope += noise->curve;
  noise->level += noise->slope;
  // noise->level is between -1 and 1, scale it to the desired range
  return min_val + (max_val - min_val) * (noise->level + 1.0f) / 2.0f;
}

#endif /* NOISE_LIB */
