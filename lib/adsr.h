
#ifndef ADSR_LIB
#define ADSR_LIB 1

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

enum envState { env_idle = 0, env_attack, env_decay, env_sustain, env_release };

typedef struct ADSR {
  float attack;   // milliseconds
  float decay;    // milliseconds
  float sustain;  // level
  float release;  // milliseconds
  float level;
  float level_attack;
  float level_release;
  float level_start;
  float start_time;
  float shape;
  float level_min;
  float level_max;
  int32_t state;
  bool gate;
} ADSR;

void ADSR_init(ADSR *adsr, float attack, float decay, float sustain,
               float release, float shape) {
  adsr->attack = attack;
  adsr->level_attack = 0;
  adsr->decay = decay;
  adsr->sustain = sustain;
  adsr->release = release;
  adsr->state = env_idle;
  adsr->level = 0;
  adsr->level_start = 0;
  adsr->start_time = 0;
  adsr->level_min = 0;
  adsr->level_max = 1.0f;
  adsr->gate = false;
  adsr->shape = shape;
}

void ADSR_set_levels(ADSR *adsr, float level_min, float level_max) {
  adsr->level_min = level_min;
  adsr->level_max = level_max;
}

void ADSR_gate(ADSR *adsr, bool gate, uint32_t current_time_ms) {
  if (adsr->gate == gate) {
    return;
  }
  adsr->gate = gate;
  adsr->start_time = current_time_ms;
  if (gate) {
    adsr->state = env_attack;
  } else {
    adsr->state = env_release;
  }
}

void ADSR_set_shape(ADSR *adsr, float shape) { adsr->shape = shape; }

void ADSR_set_attack(ADSR *adsr, float attack) { adsr->attack = attack; }

void ADSR_set_decay(ADSR *adsr, float decay) { adsr->decay = decay; }

void ADSR_set_sustain(ADSR *adsr, float sustain) { adsr->sustain = sustain; }

void ADSR_set_release(ADSR *adsr, float release) { adsr->release = release; }

float ADSR_process(ADSR *adsr, float current_time_ms) {
  if (adsr->state == env_attack) {
    int32_t elapsed = current_time_ms - adsr->start_time;
    float curve_shape = adsr->attack / adsr->shape;
    adsr->level =
        adsr->level_start +
        (1.0 - adsr->level_start) * (1.0 - exp(-1.0 * (elapsed / curve_shape)));
    adsr->level_attack = adsr->level;
    adsr->level_release = adsr->level;
    if (elapsed >= adsr->attack) {
      adsr->state = env_decay;
      adsr->start_time = current_time_ms;
    }
  }

  if (adsr->state == env_decay) {
    int32_t elapsed = current_time_ms - adsr->start_time;
    if (elapsed >= adsr->decay) {
      adsr->state = env_sustain;
      adsr->start_time = current_time_ms - (elapsed - adsr->decay);
    } else {
      float curve_shape = adsr->decay / adsr->shape;
      adsr->level = adsr->sustain + (adsr->level_attack - adsr->sustain) *
                                        exp(-1.0 * (elapsed / curve_shape));
      adsr->level_release = adsr->level;
    }
  }

  if (adsr->state == env_sustain) {
    adsr->level = adsr->level_release;
  }

  if (adsr->state == env_release) {
    int32_t elapsed = current_time_ms - adsr->start_time;
    if (elapsed >= adsr->release * 2) {
      adsr->state = env_idle;
      adsr->level = 0;
    } else {
      float curve_shape = adsr->release / adsr->shape;
      adsr->level = adsr->level_release * exp(-1.0 * (elapsed / curve_shape));
    }
  }

  // scale the level to the range [level_min, level_max]
  return adsr->level_min + (adsr->level_max - adsr->level_min) * adsr->level;
}

#endif