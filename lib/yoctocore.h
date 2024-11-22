#ifndef LIB_YOCTOCORE_H
#define LIB_YOCTOCORE_H 1

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "adsr.h"
#include "dac.h"
#include "slew.h"
#include "utils.h"

#define MODE_MANUAL 0
#define MODE_PITCH 1
#define MODE_ENVELOPE 2
#define MODE_CC 3
#define MODE_MIDI_CLOCK 4
#define MODE_CLOCK 5
#define MODE_LFO 6

#define PARAM_SCENE 274204627
#define PARAM_MODE 2090515018
#define PARAM_MIN_VOLTAGE 2621916282
#define PARAM_MAX_VOLTAGE 147793276
#define PARAM_SLEW_TIME 1829861614
#define PARAM_V_OCT 277629184
#define PARAM_ROOT_NOTE 3088989278
#define PARAM_QUANTIZATION 1091620620
#define PARAM_PORTATMENTO 2099825230
#define PARAM_MIDI_CHANNEL 2841142336
#define PARAM_MIDI_PRIORITY_CHANNEL 1809248577
#define PARAM_MIDI_CC 3330665741
#define PARAM_CLOCK_TEMPO 616644629
#define PARAM_CLOCK_DIVISION 546580981
#define PARAM_LFO_PERIOD 1640348328
#define PARAM_LFO_DEPTH 2117883034
#define PARAM_LFO_WAVEFORM 889877196
#define PARAM_ATTACK 4070033917
#define PARAM_DECAY 256485099
#define PARAM_SUSTAIN 2977350188
#define PARAM_RELEASE 1050875750
#define PARAM_LINKED_TO 303481310

float clock_divisions[19] = {1.0 / 512.0, 1.0 / 256.0, 1.0 / 128.0, 1.0 / 64.0,
                             1.0 / 32.0,  1.0 / 16.0,  1.0 / 8.0,   1.0 / 4.0,
                             1.0 / 2.0,   1.0,         2.0,         3.0,
                             4.0,         6.0,         8.0,         12.0,
                             16.0,        24.0,        48.0};

typedef struct Config {
  uint8_t mode;
  float min_voltage;
  float max_voltage;
  float slew_time;
  float v_oct;
  uint8_t root_note;
  uint16_t quantization;
  float portamento;
  uint8_t midi_channel;
  uint8_t midi_priority_channel;
  uint8_t midi_cc;
  float clock_tempo;
  uint8_t clock_division;
  float lfo_period;
  float lfo_depth;
  uint8_t lfo_waveform;
  float attack;
  float decay;
  float sustain;
  float release;
  uint8_t linked_to;
} Config;

typedef struct Out {
  float voltage_set;
  float voltage_current;
  ADSR adsr;
  Slew slew;
  Slew portamento;
} Out;

typedef struct Yoctocore {
  // index of the scene
  uint8_t i;
  // 8 scenes of 8 outputs
  Config config[8][8];
  // 8 outputs
  Out out[8];
  // debounces
  uint32_t debounce_save;
} Yoctocore;

void Yoctocore_init(Yoctocore *self) {
  for (uint8_t output = 0; output < 8; output++) {
    for (uint8_t scene = 0; scene < 8; scene++) {
      // initialize config
      self->config[scene][output].mode = 0;
      self->config[scene][output].quantization = 0;
      self->config[scene][output].v_oct = 1;
      self->config[scene][output].root_note = 48;
      self->config[scene][output].min_voltage = 0;
      self->config[scene][output].max_voltage = 5;
      self->config[scene][output].slew_time = 0;
      self->config[scene][output].portamento = 0;
      self->config[scene][output].midi_channel = 0;
      self->config[scene][output].midi_priority_channel = 0;
      self->config[scene][output].midi_cc = 0;
      self->config[scene][output].clock_tempo = 120;
      self->config[scene][output].clock_division = 9;
      self->config[scene][output].lfo_period = 0.5;
      self->config[scene][output].lfo_depth = 0.5;
      self->config[scene][output].lfo_waveform = 0;
      self->config[scene][output].attack = 0.1;
      self->config[scene][output].decay = 0.1;
      self->config[scene][output].sustain = 0.5;
      self->config[scene][output].release = 0.5;
      self->config[scene][output].linked_to = 0;
    }
    // initialize slew
    Slew_init(&self->out[output].slew, 0, 0);
    // initialize portamento
    Slew_init(&self->out[output].portamento, 0, 0);
    // initialize adsr
    ADSR_init(&self->out[output].adsr, 100.0f, 500.0f, 0.707f, 1000.0f, 5.0f);
    // initialize voltage
    self->out[output].voltage_current = 0;
    self->out[output].voltage_set = 0;
  }
  self->debounce_save = 0;
}

void Yoctocore_set(Yoctocore *self, uint8_t scene, uint8_t output,
                   uint32_t param, float val) {
  Config *config = &self->config[scene][output];
  Out *out = &self->out[output];
  switch (param) {
    case PARAM_SCENE:
      self->i = (uint8_t)val;
      break;
    case PARAM_MODE:
      config->mode = (uint8_t)val;
      break;
    case PARAM_QUANTIZATION:
      config->quantization = (uint8_t)val;
      break;
    case PARAM_PORTATMENTO:
      config->portamento = val;
      Slew_set_duration(&out->portamento, roundf(val * 1000));
      break;
    case PARAM_V_OCT:
      config->v_oct = val;
      break;
    case PARAM_ROOT_NOTE:
      config->root_note = (uint8_t)val;
      break;
    case PARAM_MIN_VOLTAGE:
      config->min_voltage = val;
      break;
    case PARAM_MAX_VOLTAGE:
      config->max_voltage = val;
      break;
    case PARAM_SLEW_TIME:
      config->slew_time = val;
      Slew_set_duration(&out->slew, roundf(val * 1000));
      break;
    case PARAM_MIDI_CHANNEL:
      config->midi_channel = (uint8_t)val;
      break;
    case PARAM_MIDI_PRIORITY_CHANNEL:
      config->midi_priority_channel = (uint8_t)val;
      break;
    case PARAM_MIDI_CC:
      config->midi_cc = (uint8_t)val;
      break;
    case PARAM_CLOCK_TEMPO:
      config->clock_tempo = val;
      break;
    case PARAM_CLOCK_DIVISION:
      config->clock_division = (uint8_t)val;
      break;
    case PARAM_LFO_PERIOD:
      config->lfo_period = val;
      break;
    case PARAM_LFO_DEPTH:
      config->lfo_depth = val;
      break;
    case PARAM_LFO_WAVEFORM:
      config->lfo_waveform = (uint8_t)val;
      break;
    case PARAM_ATTACK:
      config->attack = val;
      out->adsr.attack = roundf(config->attack * 1000);
      break;
    case PARAM_DECAY:
      config->decay = val;
      out->adsr.decay = roundf(config->decay * 1000);
      break;
    case PARAM_SUSTAIN:
      config->sustain = val;
      out->adsr.sustain = config->sustain;
      break;
    case PARAM_RELEASE:
      config->release = val;
      out->adsr.release = roundf(config->release * 1000);
      break;
    case PARAM_LINKED_TO:
      config->linked_to = (uint8_t)val;
      break;
    default:
      return;
      break;
  }
  self->debounce_save = to_ms_since_boot(get_absolute_time());
}

float Yoctocore_get(Yoctocore *self, uint8_t scene, uint8_t output,
                    uint32_t param) {
  Config *config = &self->config[scene][output];
  switch (param) {
    case PARAM_SCENE:
      return self->i;
    case PARAM_MODE:
      return config->mode;
    case PARAM_QUANTIZATION:
      return config->quantization;
    case PARAM_PORTATMENTO:
      return config->portamento;
    case PARAM_V_OCT:
      return config->v_oct;
    case PARAM_ROOT_NOTE:
      return config->root_note;
    case PARAM_MIN_VOLTAGE:
      return config->min_voltage;
    case PARAM_MAX_VOLTAGE:
      return config->max_voltage;
    case PARAM_SLEW_TIME:
      return config->slew_time;
    case PARAM_MIDI_CHANNEL:
      return config->midi_channel;
    case PARAM_MIDI_PRIORITY_CHANNEL:
      return config->midi_priority_channel;
    case PARAM_MIDI_CC:
      return config->midi_cc;
    case PARAM_CLOCK_TEMPO:
      return config->clock_tempo;
    case PARAM_CLOCK_DIVISION:
      return config->clock_division;
    case PARAM_LFO_PERIOD:
      return config->lfo_period;
    case PARAM_LFO_DEPTH:
      return config->lfo_depth;
    case PARAM_LFO_WAVEFORM:
      return config->lfo_waveform;
    case PARAM_ATTACK:
      return config->attack;
    case PARAM_DECAY:
      return config->decay;
    case PARAM_SUSTAIN:
      return config->sustain;
    case PARAM_RELEASE:
      return config->release;
    case PARAM_LINKED_TO:
      return config->linked_to;
    default:
      return -1000;
  }
}

bool Yoctocore_save(Yoctocore *self, uint32_t current_time) {
  if (self->debounce_save == 0) {
    return false;
  }
  if (current_time - self->debounce_save < 3000) {
    return false;
  }
  self->debounce_save = 0;
  FRESULT fr;
  FIL file;
  UINT bw;
  char fname[32];
  sprintf(fname, "savefile");

  fr = f_open(&file, fname, FA_WRITE | FA_CREATE_ALWAYS);
  if (FR_OK != fr) {
    printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  uint32_t total_bytes_written = 0;

  // write current index
  fr = f_write(&file, &self->i, sizeof(uint8_t), &bw);
  if (FR_OK != fr) {
    printf("f_write error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  // write each config of each scene
  for (uint8_t scene = 0; scene < 8; scene++) {
    for (uint8_t output = 0; output < 8; output++) {
      fr = f_write(&file, &self->config[scene][output], sizeof(Config), &bw);
      if (FR_OK != fr) {
        printf("f_write error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
      }
      total_bytes_written += bw;
    }
  }

  fr = f_close(&file);
  if (FR_OK != fr) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  return true;
}

bool Yoctocore_load(Yoctocore *self) {
  FRESULT fr;
  FIL file;
  UINT br;
  char fname[32];
  sprintf(fname, "savefile");

  fr = f_open(&file, fname, FA_READ);
  if (FR_OK != fr) {
    printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }

  // read current index
  fr = f_read(&file, &self->i, sizeof(uint8_t), &br);
  if (FR_OK != fr) {
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }

  // read each config of each scene
  for (uint8_t scene = 0; scene < 8; scene++) {
    for (uint8_t output = 0; output < 8; output++) {
      fr = f_read(&file, &self->config[scene][output], sizeof(Config), &br);
      if (FR_OK != fr) {
        printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
        return false;
      }
    }
  }

  fr = f_close(&file);
  if (FR_OK != fr) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  return true;
}

void Yoctocore_process_sysex(Yoctocore *self, uint8_t *buffer) {
  int scene;
  int output;
  uint32_t param_hash;
  float val;
  int parsed = parse_wxyz(buffer, &scene, &output, &param_hash, &val);
  if (parsed == 0) {
    printf("Failed to parse input\n");
    return;
  }

  // Validate scene and output indices
  if (scene < 0 || scene >= 8 || output < 0 || output >= 8) {
    printf("invalid [%d][%d]\n", scene, output);
    return;
  }

  // set the value if parsed==4
  if (parsed == 4) {
    Yoctocore_set(self, scene, output, param_hash, val);
  } else if (parsed == 3) {
    // get the value if parsed==3
    printf("%d %d %" PRIu32 " %f\n", scene, output, param_hash,
           Yoctocore_get(self, scene, output, param_hash));
  }
}

#endif  // LIB_YOCTOCORE_H