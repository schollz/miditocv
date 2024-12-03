#ifndef LIB_YOCTOCORE_H
#define LIB_YOCTOCORE_H 1

#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "adsr.h"
#include "dac.h"
#include "lfo.h"
#include "slew.h"
#include "utils.h"

#define MODE_MANUAL 0
#define MODE_PITCH 1
#define MODE_ENVELOPE 2
#define MODE_CC 3
#define MODE_CLOCK 4
#define MODE_LFO 5
#define MODE_SEQUENCER 6
#define MODE_CODE 7

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
#define PARAM_PROBABILITY 451533126
#define PARAM_CODE 2090155648
#define PARAM_CODE_LEN 709259102

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
  uint8_t probability;
  uint16_t code_len;
  char *code;
} Config;

typedef struct NoteHeld {
  uint8_t note;
  uint32_t time_on;
} NoteHeld;

typedef struct Out {
  float voltage_set;
  float voltage_current;
  float voltage_override;
  bool voltage_do_override;
  ADSR adsr;
  Slew slew;
  Slew portamento;
  Slew slew_lfo;
  NoteHeld note_on;
  Noise noise;
  bool tuning;
  float voltage_calibration_intercept;
  float voltage_calibration_slope;
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
      self->config[scene][output].probability = 100;
      self->config[scene][output].code_len = 0;
      self->config[scene][output].code = NULL;
    }
    // initialize lfo
    Noise_init(&self->out[output].noise, time_us_32());
    // initialize slew
    Slew_init(&self->out[output].slew, 0, 0);
    // initialize slew
    Slew_init(&self->out[output].slew_lfo, 0, 0);
    // initialize portamento
    Slew_init(&self->out[output].portamento, 0, 0);
    // initialize adsr
    ADSR_init(&self->out[output].adsr, 100.0f, 500.0f, 0.707f, 1000.0f, 5.0f);
    // initialize voltage
    self->out[output].voltage_current = 0;
    self->out[output].voltage_set = 0;
    self->out[output].note_on.note = 0;
    self->out[output].note_on.time_on = 0;
    self->out[output].tuning = false;
    self->out[output].voltage_override = 0;
    self->out[output].voltage_do_override = false;
  }
  self->debounce_save = 0;
}

void Yoctocore_add_code(Yoctocore *self, uint8_t scene, uint8_t output,
                        char *code, uint16_t code_len, bool append) {
  if (code_len == 0) {
    return;
  }

  if (self->config[scene][output].code == NULL || !append) {
    // Allocate memory and copy the new code
    self->config[scene][output].code = (char *)malloc(code_len);
    if (self->config[scene][output].code == NULL) {
      // Handle memory allocation failure
      printf("failed to allocate memory\n");
      return;
    }
    memcpy(self->config[scene][output].code, code, code_len);
    self->config[scene][output].code_len = code_len;
  } else {
    // Append the new code to the existing code
    uint16_t new_code_len = self->config[scene][output].code_len + code_len;
    char *new_code = (char *)malloc(new_code_len);
    if (new_code == NULL) {
      // Handle memory allocation failure
      printf("failed to allocate memory\n");
      return;
    }
    memcpy(new_code, self->config[scene][output].code,
           self->config[scene][output].code_len);
    memcpy(new_code + self->config[scene][output].code_len, code, code_len);
    free(self->config[scene][output].code);
    self->config[scene][output].code = new_code;
    self->config[scene][output].code_len = new_code_len;
    printf("newlen: %d\n", new_code_len);
  }
}

#define CODE_CHUNK_SIZE \
  36  // Total buffer size including "LS"/"LE"/"LN", scene, and output

void Yoctocore_print_code(Yoctocore *self, uint8_t scene, uint8_t output) {
  if (self->config[scene][output].code == NULL) {
    printf("LE%d%d\n", scene, output);
    return;
  }

  uint16_t code_len = self->config[scene][output].code_len;
  uint16_t i = 0;

  while (i < code_len) {
    // Create a buffer with CODE_CHUNK_SIZE
    char buffer[CODE_CHUNK_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Determine prefix: LS, LE, or LN
    if (i == 0 && code_len - i <= (CODE_CHUNK_SIZE - 4)) {
      // First and last chunk
      buffer[0] = 'L';
      buffer[1] = 'E';
    } else if (i == 0) {
      // First chunk
      buffer[0] = 'L';
      buffer[1] = 'S';
    } else if (code_len - i <= (CODE_CHUNK_SIZE - 4)) {
      // Last chunk
      buffer[0] = 'L';
      buffer[1] = 'E';
    } else {
      // Intermediate chunk
      buffer[0] = 'L';
      buffer[1] = 'N';
    }

    // Add the scene and output identifiers
    buffer[2] = '0' + scene;
    buffer[3] = '0' + output;

    // Calculate the chunk size for code data
    uint16_t chunk_size = (code_len - i > (CODE_CHUNK_SIZE - 4))
                              ? (CODE_CHUNK_SIZE - 4)
                              : (code_len - i);

    // Copy code data into the buffer, starting after the header
    memcpy(&buffer[4], &self->config[scene][output].code[i], chunk_size);
    i += chunk_size;

    // Send the buffer as SysEx
    send_buffer_as_sysex(
        buffer,
        4 + chunk_size);  // 4 for "LS"/"LE"/"LN", scene, output + chunk_size
  }
}

void Yoctocore_set(Yoctocore *self, uint8_t scene, uint8_t output,
                   uint32_t param, float val) {
  uint32_t ct = to_ms_since_boot(get_absolute_time());
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
      break;
    case PARAM_DECAY:
      config->decay = val;
      break;
    case PARAM_SUSTAIN:
      config->sustain = val;
      break;
    case PARAM_RELEASE:
      config->release = val;
      break;
    case PARAM_LINKED_TO:
      config->linked_to = (uint8_t)val;
      break;
    case PARAM_PROBABILITY:
      if (val > 100) {
        val = 100;
      }
      config->probability = (uint8_t)val;
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
    case PARAM_PROBABILITY:
      return config->probability;
    default:
      return -1000;
  }
}

bool Yoctocore_save(Yoctocore *self, uint32_t current_time) {
  if (self->debounce_save == 0) {
    return false;
  }
  if (current_time - self->debounce_save < 1000) {
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
      // write the code char
      fr = f_write(&file, self->config[scene][output].code,
                   self->config[scene][output].code_len, &bw);
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
      // read the code char
      fr = f_read(&file, self->config[scene][output].code,
                  self->config[scene][output].code_len, &br);
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
    printf("no parse: '%s'\n", buffer);
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
    if (param_hash == PARAM_CODE) {
      Yoctocore_print_code(self, scene, output);
    } else {
      printf("%d %d %" PRIu32 " %f\n", scene, output, param_hash,
             Yoctocore_get(self, scene, output, param_hash));
    }
  }
}

bool Yoctocore_set_calibration(Yoctocore *self, int output,
                               float voltage_calibration_slope,
                               float voltage_calibration_intercept) {
  FRESULT fr;
  FIL file;
  UINT bw;
  char fname[32];
  sprintf(fname, "calibration%d", output);

  fr = f_open(&file, fname, FA_WRITE | FA_CREATE_ALWAYS);
  if (FR_OK != fr) {
    printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  uint32_t total_bytes_written = 0;

  // write the calibration values
  fr = f_write(&file, &voltage_calibration_slope, sizeof(float), &bw);
  if (FR_OK != fr) {
    printf("f_write error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  fr = f_write(&file, &voltage_calibration_intercept, sizeof(float), &bw);
  if (FR_OK != fr) {
    printf("f_write error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }

  fr = f_close(&file);
  if (FR_OK != fr) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  return true;
}

void Yoctocore_get_calibrations(Yoctocore *self) {
  for (uint8_t i = 0; i < 8; i++) {
    FRESULT fr;
    FIL file;
    UINT br;
    char fname[32];
    sprintf(fname, "calibration%d", i);

    fr = f_open(&file, fname, FA_READ);
    if (FR_OK != fr) {
      printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
    fr = f_read(&file, &self->out[i].voltage_calibration_slope, sizeof(float),
                &br);
    if (FR_OK != fr) {
      printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
    fr = f_read(&file, &self->out[i].voltage_calibration_intercept,
                sizeof(float), &br);
    if (FR_OK != fr) {
      printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
    fr = f_close(&file);
    if (FR_OK != fr) {
      printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
  }
}

#endif  // LIB_YOCTOCORE_H