#ifndef KNOB_CHANGE_LIB
#define KNOB_CHANGE_LIB 1

#include <stdint.h>

typedef struct {
  int16_t last;
  uint8_t debounce;
  uint8_t threshold;
  uint8_t changed;
} KnobChange;

void KnobChange_init(KnobChange *self, uint8_t threshold) {
  self->last = -1;
  self->debounce = 0;
  self->threshold = threshold < 0 ? -threshold : threshold;
  self->changed = 0;
}

int16_t KnobChange_update(KnobChange *self, int16_t val) {
  if (self->debounce) {
    self->debounce--;
  }

  if (self->last == -1) {
    self->last = val;
  }

  int16_t diff = val - self->last;
  if (diff > self->threshold || diff < -self->threshold) {
    self->changed = 1;
    self->last = val;
    self->debounce = 5;
  } else if (self->debounce > 0) {
    self->last = val;
    self->changed = 1;
  } else {
    self->changed = 0;
    return -1;
  }

  return self->last;
}

int16_t KnobChange_get(KnobChange *self) {
  if (self->changed) {
    self->changed = 0;
    return self->last;
  }
  return -1;
}

#endif
