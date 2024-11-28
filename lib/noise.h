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

float LFNoise2(Noise *noise, float current_time, float freq) {
  // Calculate the time interval in seconds based on the frequency
  float interval = 1.0f / freq;  // Interval in seconds

  // Check if it's time to update the noise value
  if (current_time - noise->last_update_time >= interval) {
    // Update values for the next segment
    float prev_value = noise->m_nextvalue;  // Previous random value
    noise->m_nextvalue = frand2(noise);     // Generate new random value
    float midpoint = (prev_value + noise->m_nextvalue) * 0.5f;  // Midpoint

    // Calculate the curve for a parabolic trajectory
    float fseglen = interval;  // Segment length in seconds
    noise->curve = 2.0f * (midpoint - noise->level - fseglen * noise->slope) /
                   (fseglen * fseglen + fseglen);

    // Update last update time
    noise->last_update_time = current_time;
  }

  // Calculate the current noise level
  float t = current_time - noise->last_update_time;  // Time since last update
  noise->slope += noise->curve;                      // Update the slope
  noise->level += noise->slope;                      // Update the level

  // Ensure the level stays in range [-1, 1]
  if (noise->level > 1.0f) noise->level = 1.0f;
  if (noise->level < -1.0f) noise->level = -1.0f;

  // Return the current noise level
  return noise->level;
}
#endif /* NOISE_LIB */
