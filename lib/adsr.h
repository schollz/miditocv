
#ifndef ADSR_LIB
#define ADSR_LIB 1

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
  int32_t state;
  bool gate;
} ADSR;

ADSR *ADSR_malloc(float attack, float decay, float sustain, float release,
                  float shape) {
  ADSR *adsr = (ADSR *)malloc(sizeof(ADSR));
  adsr->attack = attack;
  adsr->level_attack = 0;
  adsr->decay = decay;
  adsr->sustain = sustain;
  adsr->release = release;
  adsr->state = env_idle;
  adsr->level = 0;
  adsr->level_start = 0;
  adsr->start_time = 0;
  adsr->gate = false;
  adsr->shape = shape;
  return adsr;
}

void ADSR_free(ADSR *adsr) { free(adsr); }

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

void ADSR_process(ADSR *adsr, float current_time_ms) {
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
      adsr->start_time = adsr->attack;
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
}

#endif