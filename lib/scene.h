typedef struct Output {
  uint8_t mode;
  float min_voltage;
  float max_voltage;
  float slew_time;
  uint8_t quantization;
  uint8_t midi_channel;
  uint8_t midi_cc;
  float clock_tempo;
  uint8_t clock_division;
  uint8_t lfo_waveform;
  float lfo_period;
  float lfo_depth;
} Output;

typedef struct Scene {
  Output output[8];
} Scene;

Scene scene[8];

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

  if (strcmp(buffer, "get_scene") == 0) {
    // send scene data
    send_buffer_as_sysex("send_scene", 10);
    return;
  }

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

  // Update the appropriate field based on the parameter name
  bool did_update = false;
  if (strcmp(param, "minvoltage") == 0) {
    scene[scene_num].output[output_num].min_voltage =
        util_clamp(val, -5.0f, 10.0f);
    did_update = true;
  } else if (strcmp(param, "maxvoltage") == 0) {
    scene[scene_num].output[output_num].max_voltage =
        util_clamp(val, -5.0f, 10.0f);
    did_update = true;
  } else if (strcmp(param, "slewtime") == 0) {
    scene[scene_num].output[output_num].slew_time =
        util_clamp(val, 0.0f, 10.0f);
    did_update = true;
  } else if (strcmp(param, "quantization") == 0) {
    scene[scene_num].output[output_num].quantization = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "midichannel") == 0) {
    scene[scene_num].output[output_num].midi_channel =
        (uint8_t)util_clamp(val, 0.0f, 16.0f);
    did_update = true;
  } else if (strcmp(param, "midicc") == 0) {
    scene[scene_num].output[output_num].midi_cc =
        (uint8_t)util_clamp(val, 0.0f, 127.0f);
    did_update = true;
  } else if (strcmp(param, "clocktempo") == 0) {
    scene[scene_num].output[output_num].clock_tempo =
        util_clamp(val, 20.0f, 300.0f);
    did_update = true;
  } else if (strcmp(param, "clockdivision") == 0) {
    scene[scene_num].output[output_num].clock_division = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "lfowaveform") == 0) {
    scene[scene_num].output[output_num].lfo_waveform = (uint8_t)val;
    did_update = true;
  } else if (strcmp(param, "lfoperiod") == 0) {
    scene[scene_num].output[output_num].lfo_period =
        util_clamp(val, 0.001f, 10.0f);
    did_update = true;
  } else if (strcmp(param, "lfodepth") == 0) {
    scene[scene_num].output[output_num].lfo_depth = util_clamp(val, 0.0f, 1.0f);
    did_update = true;
  } else {
    printf("Unknown parameter: %s\n", param);
  }

  if (did_update) {
    printf("scene %d output %d %s to %f\n", scene_num, output_num, param, val);
  }
}
