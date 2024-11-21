
#ifndef LIB_SCALES_H
#define LIB_SCALES_H 1
#include <stdint.h>

#define MAX_INTERVALS 16
#define MAX_SCALES 50

typedef struct {
  uint8_t intervals[MAX_INTERVALS];
  uint8_t interval_count;
} Scale;

float SCALE_CHROMATIC[13] = {
    0.0f,          1.0f / 12.0f,  2.0f / 12.0f, 3.0f / 12.0f, 4.0f / 12.0f,
    5.0f / 12.0f,  6.0f / 12.0f,  7.0f / 12.0f, 8.0f / 12.0f, 9.0f / 12.0f,
    10.0f / 12.0f, 11.0f / 12.0f, 1.0f};

Scale SCALES[MAX_SCALES] = {
    {// Chromatic
     {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12},
     13},
    {// Major
     {0, 2, 4, 5, 7, 9, 11, 12},
     8},
    {// Natural Minor
     {0, 2, 3, 5, 7, 8, 10, 12},
     8},
    {// Harmonic Minor
     {0, 2, 3, 5, 7, 8, 11, 12},
     8},
    {// Melodic Minor
     {0, 2, 3, 5, 7, 9, 11, 12},
     8},
    {// Dorian
     {0, 2, 3, 5, 7, 9, 10, 12},
     8},
    {// Phrygian
     {0, 1, 3, 5, 7, 8, 10, 12},
     8},
    {// Lydian
     {0, 2, 4, 6, 7, 9, 11, 12},
     8},
    {// Mixolydian
     {0, 2, 4, 5, 7, 9, 10, 12},
     8},
    {// Locrian
     {0, 1, 3, 5, 6, 8, 10, 12},
     8},
    {// Whole Tone
     {0, 2, 4, 6, 8, 10, 12},
     7},
    {// Major Pentatonic
     {0, 2, 4, 7, 9, 12},
     6},
    {// Minor Pentatonic
     {0, 3, 5, 7, 10, 12},
     6},
    {// Major Bebop
     {0, 2, 4, 5, 7, 8, 9, 11, 12},
     9},
    {// Altered Scale
     {0, 1, 3, 4, 6, 8, 10, 12},
     8},
    {// Dorian Bebop
     {0, 2, 3, 4, 5, 7, 9, 10, 12},
     9},
    {// Mixolydian Bebop
     {0, 2, 4, 5, 7, 9, 10, 11, 12},
     9},
    {// Blues Scale
     {0, 3, 5, 6, 7, 10, 12},
     7},
    {// Diminished Whole Half
     {0, 2, 3, 5, 6, 8, 9, 11, 12},
     9},
    {// Diminished Half Whole
     {0, 1, 3, 4, 6, 7, 9, 10, 12},
     9},
    {// Neapolitan Major
     {0, 1, 3, 5, 7, 9, 11, 12},
     8},
    {// Hungarian Major
     {0, 3, 4, 6, 7, 9, 10, 12},
     8},
    {// Harmonic Major
     {0, 2, 4, 5, 7, 8, 11, 12},
     8},
    {// Hungarian Minor
     {0, 2, 3, 6, 7, 8, 11, 12},
     8},
    {// Lydian Minor
     {0, 2, 4, 6, 7, 8, 10, 12},
     8},
    {// Neapolitan Minor
     {0, 1, 3, 5, 7, 8, 11, 12},
     8},
    {// Major Locrian
     {0, 2, 4, 5, 6, 8, 10, 12},
     8},
    {// Leading Whole Tone
     {0, 2, 4, 6, 8, 10, 11, 12},
     8},
    {// Six Tone Symmetrical
     {0, 1, 4, 5, 8, 9, 11, 12},
     8},
    {// Balinese
     {0, 1, 3, 7, 8, 12},
     6},
    {// Persian
     {0, 1, 4, 5, 6, 8, 11, 12},
     8},
    {// East Indian Purvi
     {0, 1, 4, 6, 7, 8, 11, 12},
     8},
    {// Oriental
     {0, 1, 4, 5, 6, 9, 10, 12},
     8},
    {// Double Harmonic
     {0, 1, 4, 5, 7, 8, 11, 12},
     8},
    {// Enigmatic
     {0, 1, 4, 6, 8, 10, 11, 12},
     8},
    {// Overtone
     {0, 2, 4, 6, 7, 9, 10, 12},
     8},
    {// Eight Tone Spanish
     {0, 1, 3, 4, 5, 6, 8, 10, 12},
     9},
    {// Prometheus
     {0, 2, 4, 6, 9, 10, 12},
     7},
    {// Gagaku Rittsu Sen Pou
     {0, 2, 5, 7, 9, 10, 12},
     7},
    {// In Sen Pou
     {0, 1, 5, 2, 8, 12},
     6},
    {// Okinawa
     {0, 4, 5, 7, 11, 12},
     6},
};

float scale_quantize_voltage(uint8_t scale, float voltage) {
  if (scale == 0) {
    // no quantization
    return voltage;
  }
  scale--;
  float voltage_octave = 0;
  while (voltage >= 1.0f) {
    voltage_octave += 1.0f;
    voltage -= 1.0f;
  }
  while (voltage < 0.0f) {
    voltage_octave -= 1.0f;
    voltage += 1.0f;
  }
  // find the closest SCALE_CHROMATIC for the scale
  float closest = 0.0f;
  float closest_diff = 1.0f;
  for (uint8_t i = 0; i < SCALES[scale].interval_count; i++) {
    float diff = SCALE_CHROMATIC[SCALES[scale].intervals[i]] - voltage;
    if (diff < 0.0f) {
      diff = -diff;
    }
    if (diff < closest_diff) {
      closest_diff = diff;
      closest = SCALE_CHROMATIC[SCALES[scale].intervals[i]];
    }
  }
  return closest + voltage_octave;
}
#endif