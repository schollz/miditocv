#ifndef SCENE_LIB
#define SCENE_LIB 1

#include "slew.h"

typedef struct Output {
  uint8_t mode;
  uint8_t quantization;
  float min_voltage;
  float max_voltage;
  float slew_time;
  uint8_t midi_channel;
  uint8_t midi_priority_channel;
  uint8_t midi_cc;
  float clock_tempo;
  uint8_t clock_division;
  float lfo_period;
  float lfo_depth;
  uint8_t lfo_waveform;
} Output;

typedef struct OutputProcess {
  Slew slew;
} OutputProcess;

typedef struct Scene {
  Output output[8];                 // data that is saved
  OutputProcess output_process[8];  // data that is processed
} Scene;

Scene scenes[8];
bool scene_dirty[9] = {false, false, false, false, false,
                       false, false, false, false};
uint32_t debounce_scene_save = 0;
typedef struct StateData {
  uint32_t magic;
  uint8_t scene;
} StateData;

StateData state;

void Scenes_init() {
  for (uint8_t scene = 0; scene < 8; scene++) {
    for (uint8_t output = 0; output < 8; output++) {
      scenes[scene].output[output].mode = 0;
      scenes[scene].output[output].quantization = 0;
      scenes[scene].output[output].min_voltage = 0;
      scenes[scene].output[output].max_voltage = 5;
      scenes[scene].output[output].slew_time = 0;
      scenes[scene].output[output].midi_channel = 0;
      scenes[scene].output[output].midi_priority_channel = 0;
      scenes[scene].output[output].midi_cc = 0;
      scenes[scene].output[output].clock_tempo = 120;
      scenes[scene].output[output].clock_division = 0;
      scenes[scene].output[output].lfo_period = 0.5;
      scenes[scene].output[output].lfo_depth = 0.5;
      scenes[scene].output[output].lfo_waveform = 0;
      Slew_init(&scenes[scene].output_process[output].slew, 0, 0);
    }
  }
}

void Scene_marshal(const Scene *scene, uint8_t *buffer) {
  for (int i = 0; i < 8; i++) {
    memcpy(buffer + i * sizeof(Output), &scene[i].output, sizeof(Output));
  }
}

void Scene_unmarshal(Scene *scene, const uint8_t *buffer) {
  for (int i = 0; i < 8; i++) {
    memcpy(&scene[i].output, buffer + i * sizeof(Output), sizeof(Output));
  }
}

bool parse_wxyz(const char *input, int *W, int *X, char *Y, float *Z) {
  // Temporary variables to store parts of the string
  char tempY[256];  // Assumes Y will not exceed 256 characters

  // Use sscanf to parse the input string
  int matched = sscanf(input, "%d_%d_%255[^_]_%f", W, X, tempY, Z);

  // Check if all parts were successfully parsed
  if (matched == 4) {
    // Copy the parsed string Y into the provided Y pointer
    strcpy(Y, tempY);
    return true;
  }

  // Return false if parsing failed
  return false;
}

void Scene_update_with_sysex(uint8_t *buffer) {
  int scene_num;
  int output_num;
  char param[256];
  float val;

  // Parse the sysex message
  bool parsed =
      parse_wxyz((char *)buffer, &scene_num, &output_num, param, &val);
  if (!parsed) {
    printf("%d %d %s %f\n", scene_num, output_num, param, val);
    return;
  }

  // Validate scene and output indices
  if (scene_num < 0 || scene_num >= 8 || output_num < 0 || output_num >= 8) {
    printf("invalid [%d][%d]\n", scene_num, output_num);
    return;
  }

  if (val < -9.0f && val > -11.0f) {
    // return the values
    if (strcmp(param, "mode") == 0) {
      printf("%d %d mode %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].mode);
    } else if (strcmp(param, "minvoltage") == 0) {
      printf("%d %d min_voltage %2.3f\n", scene_num, output_num,
             scenes[scene_num].output[output_num].min_voltage);
    } else if (strcmp(param, "maxvoltage") == 0) {
      printf("%d %d max_voltage %2.3f\n", scene_num, output_num,
             scenes[scene_num].output[output_num].max_voltage);
    } else if (strcmp(param, "slewtime") == 0) {
      printf("%d %d slew_time %2.3f\n", scene_num, output_num,
             scenes[scene_num].output[output_num].slew_time);
    } else if (strcmp(param, "quantization") == 0) {
      printf("%d %d quantization %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].quantization);
    } else if (strcmp(param, "midichannel") == 0) {
      printf("%d %d midi_channel %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].midi_channel);
    } else if (strcmp(param, "midiprioritychannel") == 0) {
      printf("%d %d midi_priority_channel %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].midi_priority_channel);
    } else if (strcmp(param, "midicc") == 0) {
      printf("%d %d midi_cc %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].midi_cc);
    } else if (strcmp(param, "clocktempo") == 0) {
      printf("%d %d clock_tempo %2.3f\n", scene_num, output_num,
             scenes[scene_num].output[output_num].clock_tempo);
    } else if (strcmp(param, "clockdivision") == 0) {
      printf("%d %d clock_division %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].clock_division);
    } else if (strcmp(param, "lfowaveform") == 0) {
      printf("%d %d lfo_waveform %d\n", scene_num, output_num,
             scenes[scene_num].output[output_num].lfo_waveform);
    } else if (strcmp(param, "lfoperiod") == 0) {
      printf("%d %d lfo_period %2.3f\n", scene_num, output_num,
             scenes[scene_num].output[output_num].lfo_period);
    } else if (strcmp(param, "lfodepth") == 0) {
      printf("%d %d lfo_depth %2.3f\n", scene_num, output_num,
             scenes[scene_num].output[output_num].lfo_depth);
      // state data
    } else if (strcmp(param, "scene") == 0) {
      printf("scene%d\n", state.scene);
    } else {
      printf("Unknown parameter: %s\n", param);
    }

    return;
  }

  // Update the appropriate field based on the parameter name
  bool did_update = false;
  bool did_update_state = false;
  if (strcmp(param, "mode") == 0) {
    scenes[scene_num].output[output_num].mode = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "minvoltage") == 0) {
    scenes[scene_num].output[output_num].min_voltage = val;
    did_update = true;
  } else if (strcmp(param, "maxvoltage") == 0) {
    val = util_clamp(val, -5.0f, 10.0f);
    scenes[scene_num].output[output_num].max_voltage = val;
    did_update = true;
  } else if (strcmp(param, "slewtime") == 0) {
    val = util_clamp(val, 0.0f, 10.0f);
    scenes[scene_num].output[output_num].slew_time = val;
    Slew_set_duration(&scenes[scene_num].output_process[output_num].slew,
                      roundf(val * 1000));
    did_update = true;
  } else if (strcmp(param, "quantization") == 0) {
    scenes[scene_num].output[output_num].quantization = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "midichannel") == 0) {
    val = util_clamp(val, 0.0f, 16.0f);
    scenes[scene_num].output[output_num].midi_channel = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "midiprioritychannel") == 0) {
    val = util_clamp(val, 0.0f, 16.0f);
    scenes[scene_num].output[output_num].midi_priority_channel = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "midicc") == 0) {
    val = util_clamp(val, 0.0f, 127.0f);
    scenes[scene_num].output[output_num].midi_cc = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "clocktempo") == 0) {
    val = util_clamp(val, 20.0f, 300.0f);
    scenes[scene_num].output[output_num].clock_tempo = val;
    did_update = true;
  } else if (strcmp(param, "clockdivision") == 0) {
    scenes[scene_num].output[output_num].clock_division = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "lfowaveform") == 0) {
    scenes[scene_num].output[output_num].lfo_waveform = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "lfoperiod") == 0) {
    val = util_clamp(val, 0.001f, 10.0f);
    scenes[scene_num].output[output_num].lfo_period = val;
    did_update = true;
  } else if (strcmp(param, "lfodepth") == 0) {
    val = util_clamp(val, 0.0f, 1.0f);
    scenes[scene_num].output[output_num].lfo_depth = val;
    did_update = true;
    // state data
  } else if (strcmp(param, "scene") == 0) {
    state.scene = (uint8_t)val;
    did_update_state = true;
  } else {
    printf("Unknown parameter: %s\n", param);
  }

  if (did_update) {
    printf("scene %d output %d %s to %f (%d)\n", scene_num, output_num, param,
           val, sizeof(Scene));
    debounce_scene_save = to_ms_since_boot(get_absolute_time());
  }
  if (did_update_state) {
    printf("state scene to %d\n", state.scene);
    debounce_scene_save = to_ms_since_boot(get_absolute_time());
  }
}

void Scene_save_data_sdcard() {
  if (debounce_scene_save == 0) {
    return;
  }
  if (to_ms_since_boot(get_absolute_time()) - debounce_scene_save < 3000) {
    return;
  }
  uint64_t start_time_us = time_us_64();
  debounce_scene_save = 0;
  FRESULT fr;
  FIL file; /* File object */
  char fname[32];

  sprintf(fname, "savefile2");
  // f_open and overwrite if exists
  fr = f_open(&file, fname, FA_WRITE | FA_CREATE_ALWAYS);
  if (FR_OK != fr) {
    printf("f_open error: %s (%d)\n", FRESULT_str(fr), fr);
    return;
  }
  uint32_t total_bytes_written = 0;
  for (uint8_t i = 0; i < 8; i++) {
    UINT bw;
    if (f_write(&file, &scenes[i], sizeof(Scene), &bw)) {
      printf("problem writing scene %d\n", i);
      return;
    }
    total_bytes_written += bw;
  }
  f_close(&file);
  printf("[SaveFile] wrote %d bytes in %lld us\n", total_bytes_written,
         time_us_64() - start_time_us);
}

void Scene_save_data() {
  if (debounce_scene_save == 0) {
    return;
  }
  if (to_ms_since_boot(get_absolute_time()) - debounce_scene_save < 3000) {
    return;
  }
  uint64_t start_time_us = time_us_64();
  debounce_scene_save = 0;
  uint32_t interrupts = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_erase(FLASH_TARGET_OFFSET + FLASH_SECTOR_SIZE, FLASH_SECTOR_SIZE);

  // save state data
  uint8_t flash_data[FLASH_PAGE_SIZE];
  memcpy(flash_data, &state, sizeof(StateData));
  flash_range_program(FLASH_TARGET_OFFSET + 8 * FLASH_PAGE_SIZE * 2, flash_data,
                      FLASH_PAGE_SIZE);

  for (size_t scene_num = 0; scene_num < 8; scene_num++) {
    uint8_t flash_data2[FLASH_PAGE_SIZE * 2];
    // copy each output to the flash data
    for (int i = 0; i < 8; i++) {
      memcpy(flash_data2 + i * sizeof(Output), &scenes[scene_num].output[i],
             sizeof(Output));
    }
    flash_range_program(FLASH_TARGET_OFFSET + scene_num * FLASH_PAGE_SIZE * 2,
                        flash_data2, 2 * FLASH_PAGE_SIZE);
  }
  restore_interrupts(interrupts);

  // printf("saving data(%d) (%d) (%d)\n", FLASH_PAGE_SIZE, FLASH_SECTOR_SIZE,
  //        8 * sizeof(Output));
  printf("saved data in %lld us\n", time_us_64() - start_time_us);
}

void Scene_load_data_sdcard() {
  FIL fil; /* File object */
  char fname[32];
  uint64_t start_time_us = time_us_64();
  unsigned int bytes_read;
  sprintf(fname, "savefile2");
  if (f_open(&fil, fname, FA_READ)) {
    printf("[SaveFile] no save file, skipping ");
    return;
  } else {
    for (uint8_t i = 0; i < 8; i++) {
      UINT br;
      if (f_read(&fil, &scenes[i], sizeof(Scene), &br)) {
        printf("[SaveFile] problem reading scene %d\n", i);
      }
      bytes_read += br;
    }
  }
  f_close(&fil);
  printf("read %d bytes in %lld us\n", bytes_read,
         time_us_64() - start_time_us);
}

const uint8_t *flash_target_contents_data =
    (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
void Scene_load_data() {
  // load state data
  uint8_t flash_data[FLASH_PAGE_SIZE];
  for (size_t i = 0; i < FLASH_PAGE_SIZE; i++) {
    flash_data[i] = flash_target_contents_data[i + 8 * FLASH_PAGE_SIZE * 2];
  }
  memcpy(&state, flash_data, sizeof(StateData));
  if (state.scene < 0 || state.scene >= 8) {
    state.scene = 0;
  }

  for (size_t scene_num = 0; scene_num < 8; scene_num++) {
    uint8_t flash_data[FLASH_PAGE_SIZE * 2];
    for (size_t i = 0; i < FLASH_PAGE_SIZE * 2; i++) {
      flash_data[i] =
          flash_target_contents_data[i + scene_num * FLASH_PAGE_SIZE * 2];
    }
    // copy each output from the flash data
    for (int i = 0; i < 8; i++) {
      memcpy(&scenes[scene_num].output[i], flash_data + i * sizeof(Output),
             sizeof(Output));
      if (state.magic != 123456) {
        // reset it
        scenes[scene_num].output[i].mode = 0;
        scenes[scene_num].output[i].quantization = 0;
        scenes[scene_num].output[i].min_voltage = 0;
        scenes[scene_num].output[i].max_voltage = 5;
        scenes[scene_num].output[i].slew_time = 0;
        scenes[scene_num].output[i].midi_channel = 0;
        scenes[scene_num].output[i].midi_priority_channel = 0;
        scenes[scene_num].output[i].midi_cc = 0;
        scenes[scene_num].output[i].clock_tempo = 120;
        scenes[scene_num].output[i].clock_division = 0;
        scenes[scene_num].output[i].lfo_period = 0.5;
        scenes[scene_num].output[i].lfo_depth = 0.5;
        scenes[scene_num].output[i].lfo_waveform = 0;
      }
    }
  }
  state.magic = 123456;
}

#endif