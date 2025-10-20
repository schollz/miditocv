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
#include "taptempo.h"
#include "utils.h"

#define MODE_NOTE 0
#define MODE_ENVELOPE 1
#define MODE_GATE 2
#define MODE_KEY_PRESSURE 3
#define MODE_CONTROL_CHANGE 4
#define MODE_PROGRAM_CHANGE 5
#define MODE_CHANNEL_PRESSURE 6
#define MODE_PITCH_BEND 7
#define MODE_CLOCK 8
#define MODE_LFO 9
#define MODE_CODE 10

#define PARAM_MODE 0
#define PARAM_MIN_VOLTAGE 1
#define PARAM_MAX_VOLTAGE 2
#define PARAM_SLEW_TIME 3
#define PARAM_V_OCT 4
#define PARAM_ROOT_NOTE 5
#define PARAM_QUANTIZATION 6
#define PARAM_PORTAMENTO 7
#define PARAM_MIDI_CHANNEL 8
#define PARAM_MIDI_PRIORITY_CHANNEL 9
#define PARAM_MIDI_CC 10
#define PARAM_CLOCK_TEMPO 11
#define PARAM_CLOCK_DIVISION 12
#define PARAM_LFO_PERIOD 13
#define PARAM_LFO_DEPTH 14
#define PARAM_LFO_WAVEFORM 15
#define PARAM_ATTACK 16
#define PARAM_DECAY 17
#define PARAM_SUSTAIN 18
#define PARAM_RELEASE 19
#define PARAM_LINKED_TO 20
#define PARAM_PROBABILITY 21
#define PARAM_NOTE_TUNING 22
#define PARAM_SCENE 23
#define PARAM_CODE 24

#define MAGIC_UINT16 0x5A5A

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
  int16_t note_tuning;
  uint16_t magic;
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
  int8_t mode_last;
  bool code_updated;
  uint8_t code_updated_scene;  // Track which scene the code update is for
  bool clock_disabled;
  bool lfo_disabled;
  bool lua_panic;  // Flag to indicate Lua code has panicked
  TapTempo taptempo;
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
  float global_tempo;
  uint32_t miditocv_getting;
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
      self->config[scene][output].note_tuning = 0;
      self->config[scene][output].magic = MAGIC_UINT16;
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
    self->out[output].mode_last = -1;
    self->out[output].code_updated = false;
    self->out[output].code_updated_scene = 0;
    self->out[output].clock_disabled = false;
    self->out[output].lfo_disabled = false;
    self->out[output].lua_panic = false;
    TapTempo_init(&self->out[output].taptempo);
  }
  self->debounce_save = 0;
  self->i = 0;
  self->global_tempo = 120;
}

void Yoctocore_schedule_save(Yoctocore *self) {
  self->debounce_save = to_ms_since_boot(get_absolute_time());
}

char *code_added = NULL;
size_t code_added_len = 0;

void Yoctocore_add_code(Yoctocore *self, uint8_t scene, uint8_t output,
                        char *code, uint16_t code_len, bool append,
                        bool finish) {
  if (code_len == 0) {
    return;
  }

  if (code_added == NULL || !append) {
    if (code_added != NULL) {
      free(code_added);
      code_added = NULL;
    }
    // Allocate memory and copy the new code
    code_added = (char *)malloc(code_len);
    if (code_added == NULL) {
      // Handle memory allocation failure
      printf("failed to allocate memory\n");
      return;
    }
    memcpy(code_added, code, code_len);
    code_added_len = code_len;
  } else {
    // Append the new code to the existing code
    size_t new_code_len = code_added_len + code_len;
    char *new_code = (char *)malloc(new_code_len + 1);
    if (new_code == NULL) {
      // Handle memory allocation failure
      printf("failed to allocate memory\n");
      return;
    }
    memcpy(new_code, code_added, code_added_len);
    memcpy(new_code + code_added_len, code, code_len);
    free(code_added);
    code_added = new_code;
    if (new_code_len > 0) {
      // null terminate
      code_added[new_code_len] = '\0';
    }
    code_added_len = new_code_len;
  }

  if (finish) {
    // write to disk
    FRESULT fr;
    FIL file;
    UINT bw;
    char fname[32];
    snprintf(fname, sizeof(fname), "scene%d_output%d.lua", scene + 1,
             output + 1);
    fr = f_open(&file, fname, FA_WRITE | FA_CREATE_ALWAYS);
    if (fr != FR_OK) {
      printf("f_open error: %s (%d): %s\n", FRESULT_str(fr), fr, fname);
      free(code_added);
      code_added = NULL;
      return;
    }
    fr = f_write(&file, code_added, code_added_len, &bw);
    if (fr != FR_OK || bw != code_added_len) {
      printf("f_write error: %s (%d)\n", FRESULT_str(fr), fr);
      free(code_added);
      code_added = NULL;
      return;
    }
    fr = f_close(&file);
    if (fr != FR_OK) {
      printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    }
    free(code_added);
    code_added = NULL;
    // set code to be updated
    self->out[output].code_updated = true;
    self->out[output].code_updated_scene = scene;
    printf("[%d%d] code %d bytes\n", scene, output, code_added_len);
  }
}

bool Yoctocore_do_load_code(Yoctocore *self, uint8_t scene, uint8_t output,
                            char **code, FSIZE_t *code_len) {
  FRESULT fr;
  FIL file;
  UINT br;
  char fname[32];

  // Create the filename (1-indexed)
  snprintf(fname, sizeof(fname), "scene%d_output%d.lua", scene + 1, output + 1);

  // Open the file
  fr = f_open(&file, fname, FA_READ);
  if (fr != FR_OK) {
    printf("f_open error: %s (%d): %s\n", FRESULT_str(fr), fr, fname);
    return false;
  }

  // Get the file size
  *code_len = f_size(&file);
  if (*code_len == 0) {
    printf("%s is empty\n", fname);
    f_close(&file);
    return false;
  }

  // Allocate memory for the file contents
  *code = (char *)malloc(*code_len + 1);
  if (*code == NULL) {
    printf("Failed to allocate memory\n");
    f_close(&file);
    return false;
  }

  // Read the file contents
  fr = f_read(&file, *code, *code_len, &br);
  if (fr != FR_OK || br != *code_len) {
    printf("f_read error: %s (%d)\n", FRESULT_str(fr), fr);
    free(*code);
    *code = NULL;
    f_close(&file);
    return false;
  }

  // Null-terminate the code
  (*code)[*code_len] = '\0';
  printf("[%d%d] code %d bytes:\n----\n%s\n----\n", scene, output, *code_len,
         *code);

  // Close the file
  fr = f_close(&file);
  if (fr != FR_OK) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    free(*code);
    *code = NULL;
    return false;
  }

  return true;
}

#define CODE_CHUNK_SIZE \
  36  // Total buffer size including "LS"/"LE"/"LN", scene, and output

void Yoctocore_print_code(Yoctocore *self, uint8_t scene, uint8_t output) {
  // Load code
  char *code;
  FSIZE_t code_len;
  if (!Yoctocore_do_load_code(self, scene, output, &code, &code_len)) {
    // No code file exists - send empty code response (single LE chunk with no data)
    char buffer[4];
    buffer[0] = 'L';
    buffer[1] = 'E';
    buffer[2] = '0' + scene;
    buffer[3] = '0' + output;
#ifdef INCLUDE_MIDI
    send_buffer_as_sysex(buffer, 4);
#endif
    return;
  }

  uint16_t i = 0;

  while (i < code_len) {
    // Create a buffer with CODE_CHUNK_SIZE
    char buffer[CODE_CHUNK_SIZE];
    memset(buffer, 0, sizeof(buffer));

    // Determine prefix: LS (start), LN (continuing), or LE (end)
    if (i == 0 && code_len - i <= (CODE_CHUNK_SIZE - 4)) {
      // First and only chunk (single chunk transfer)
      buffer[0] = 'L';
      buffer[1] = 'E';
    } else if (i == 0) {
      // First chunk (start)
      buffer[0] = 'L';
      buffer[1] = 'S';
    } else if (code_len - i <= (CODE_CHUNK_SIZE - 4)) {
      // Last chunk (end)
      buffer[0] = 'L';
      buffer[1] = 'E';
    } else {
      // Middle chunk (continuing)
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
    memcpy(&buffer[4], &code[i], chunk_size);
    i += chunk_size;

#ifdef INCLUDE_MIDI
    // Send the buffer as SysEx
    send_buffer_as_sysex(
        buffer,
        4 + chunk_size);  // 4 for "LS"/"LE"/"LN", scene, output + chunk_size
#endif
  }
  // free code
  free(code);
  code = NULL;
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
    case PARAM_PORTAMENTO:
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
      if (val > 0) {
        val += 29;
      }
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
    case PARAM_NOTE_TUNING:
      config->note_tuning = val;
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
    case PARAM_PORTAMENTO:
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
      float val = config->clock_tempo;
      if (val > 0) {
        val -= 29;
      }
      return val;
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
    case PARAM_NOTE_TUNING:
      return config->note_tuning;
    default:
      return -1000;
  }
}

bool Yoctocore_do_save(Yoctocore *self) {
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

bool Yoctocore_save(Yoctocore *self, uint32_t current_time) {
  if (self->debounce_save == 0) {
    return false;
  }
  if (current_time - self->debounce_save < 1000) {
    return false;
  }
  self->debounce_save = 0;
  return Yoctocore_do_save(self);
}

bool Yoctocore_load_code(Yoctocore *self, uint8_t scene, uint8_t output) {
  char *code = NULL;
  FSIZE_t code_len = 0;
  // Load the code from the file
  if (!Yoctocore_do_load_code(self, scene, output, &code, &code_len)) {
    return false;
  }
  if (code_len == 0) {
    free(code);
    code = NULL;
    return false;
  }
  // print out the code
  printf("code %d bytes\n", code_len);
  sleep_ms(100);
  printf("code: %s\n", code);
  sleep_ms(100);
  // Update the environment with the loaded code
  bool result = (luaUpdateEnvironment(output, code) == 0);
  // Free the allocated memory
  free(code);
  code = NULL;
  return result;
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
        Yoctocore_init(self);
        return false;
      }
    }
  }

  fr = f_close(&file);
  if (FR_OK != fr) {
    printf("f_close error: %s (%d)\n", FRESULT_str(fr), fr);
    Yoctocore_init(self);
    return false;
  }

  // check the magic numbers
  for (uint8_t scene = 0; scene < 8; scene++) {
    for (uint8_t output = 0; output < 8; output++) {
      if (self->config[scene][output].magic != MAGIC_UINT16) {
        printf("magic number mismatch\n");
        // initialize the config
        Yoctocore_init(self);
        return false;
      }
    }
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
      self->miditocv_getting = to_ms_since_boot(get_absolute_time());
      printf_sysex("%d %d %" PRIu32 " %2.2f\n", scene, output, param_hash,
                   Yoctocore_get(self, scene, output, param_hash));
    }
  }
}

bool Yoctocore_set_calibration(Yoctocore *self, int output,
                               float voltage_calibration_slope,
                               float voltage_calibration_intercept) {
  self->out[output].voltage_calibration_slope = voltage_calibration_slope;
  self->out[output].voltage_calibration_intercept =
      voltage_calibration_intercept;
  FRESULT fr;
  FIL file;
  UINT bw;
  char fname[32];
  sprintf(fname, "calibration%d", output);

  fr = f_open(&file, fname, FA_WRITE | FA_CREATE_ALWAYS);
  if (FR_OK != fr) {
    printf("f_open set: %s (%d) (%s)\n", FRESULT_str(fr), fr, fname);
    return false;
  }
  uint32_t total_bytes_written = 0;

  // write the calibration values
  fr = f_write(&file, &voltage_calibration_slope, sizeof(float), &bw);
  if (FR_OK != fr) {
    printf("f_write set: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  fr = f_write(&file, &voltage_calibration_intercept, sizeof(float), &bw);
  if (FR_OK != fr) {
    printf("f_write set: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }

  fr = f_close(&file);
  if (FR_OK != fr) {
    printf("f_close set: %s (%d)\n", FRESULT_str(fr), fr);
    return false;
  }
  printf("set cali for %d\n", output);
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
      printf("f_open: %s (%d) %s\n", FRESULT_str(fr), fr, fname);
      return;
    }
    fr = f_read(&file, &self->out[i].voltage_calibration_slope, sizeof(float),
                &br);
    if (FR_OK != fr) {
      printf("f_read get: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
    fr = f_read(&file, &self->out[i].voltage_calibration_intercept,
                sizeof(float), &br);
    if (FR_OK != fr) {
      printf("f_read get: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
    fr = f_close(&file);
    if (FR_OK != fr) {
      printf("f_close get: %s (%d)\n", FRESULT_str(fr), fr);
      return;
    }
  }
}

#endif  // LIB_YOCTOCORE_H