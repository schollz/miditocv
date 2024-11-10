typedef struct Output {
  uint16_t mode : 4;
  uint16_t quantization : 4;
  uint16_t clock_division : 4;
  uint16_t lfo_waveform : 4;
  float min_voltage;
  float max_voltage;
  float slew_time;
  uint8_t midi_channel;
  uint8_t midi_cc;
  float clock_tempo;
  float lfo_period;
  float lfo_depth;
} Output;

typedef struct Scene {
  Output output[8];
} Scene;

Scene scenes[8];

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
    } else {
      printf("Unknown parameter: %s\n", param);
    }

    return;
  }

  // Update the appropriate field based on the parameter name
  bool did_update = false;
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
    did_update = true;
  } else if (strcmp(param, "quantization") == 0) {
    scenes[scene_num].output[output_num].quantization = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "midichannel") == 0) {
    val = util_clamp(val, 0.0f, 16.0f);
    scenes[scene_num].output[output_num].midi_channel = (uint8_t)val;
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
  } else {
    printf("Unknown parameter: %s\n", param);
  }

  if (did_update) {
    printf("scene %d output %d %s to %f\n", scene_num, output_num, param, val);
    // // erase sector
    // pico_flash_erase();
    // // write data
    // uint8_t flash_data[FLASH_PAGE_SIZE];
    // for (int i = 0; i < 8; i++) {
    //   Scene_marshal(&scenes[i], flash_data);
    //   pico_flash_write(flash_data, i);
    // }
  }
}

void Scene_load_data() {
  //   uint8_t flash_data[FLASH_PAGE_SIZE];
  //   for (int i = 0; i < 8; i++) {
  //     pico_flash_read(flash_data, FLASH_PAGE_SIZE, i);
  //     memcpy(&scene[i], flash_data, sizeof(Scene));
  //   }
}