
#ifndef LIB_SCALES_H
#define LIB_SCALES_H 1
#include <stdint.h>

#define MAX_INTERVALS 16
#define MAX_ALT_NAMES 8
#define MAX_NAME_LENGTH 32
#define MAX_SCALES 50

typedef struct Scale {
  uint8_t intervals[MAX_INTERVALS];
  uint8_t interval_count;
} Scale;

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
#endif