#ifndef LIB_MIDI_CALLBACK_H
#define LIB_MIDI_CALLBACK_H 1

#include "dac.h"

// #define DEBUG_MIDI 1
#define MIDI_MAX_NOTES 128
#define MIDI_MAX_TIME_ON 10000  // 10 seconds
#define MIDI_RESET_EVERY_BEAT 16
#define MIDI_CLOCK_MULTIPLIER 2

uint32_t note_hit[MIDI_MAX_NOTES];
int midi_bpm_detect[7];
uint8_t midi_bpm_detect_i = 0;
bool note_on[MIDI_MAX_NOTES];

bool get_sysex_param_float_value(const char *param_name, const uint8_t *sysex,
                                 size_t length, float *out_value) {
  size_t param_len = strlen(param_name);

  // Check if the SysEx message is long enough and contains the parameter name
  if (length > param_len && memcmp(sysex, param_name, param_len) == 0) {
    // Allocate a temporary buffer for the float part
    char value_str[length - param_len + 1];

    // Copy the float part into the buffer
    for (size_t i = param_len; i < length; i++) {
      value_str[i - param_len] = sysex[i];
    }

    // Null-terminate the string
    value_str[length - param_len] = '\0';

    // Convert the extracted string to a float and store it in out_value
    *out_value = strtof(value_str, NULL);
    return true;
  }

  // Return false if the parameter name is not found or the message is invalid
  return false;
}

bool get_sysex_param_int_float_values(const char *param_name,
                                      const uint8_t *sysex, size_t length,
                                      int *out_int, float *out_value) {
  size_t param_len = strlen(param_name);

  if (length > param_len && memcmp(sysex, param_name, param_len) == 0) {
    char value_str[length - param_len + 1];

    for (size_t i = param_len; i < length; i++) {
      value_str[i - param_len] = sysex[i];
    }

    value_str[length - param_len] = '\0';

    char *token = strtok(value_str, "_");
    if (token == NULL) return false;

    *out_int = strtol(token, NULL, 10);

    token = strtok(NULL, "_");
    if (token == NULL) return false;
    *out_value = strtof(token, NULL);

    return true;
  }
  return false;
}

bool get_sysex_param_int_and_two_float_values(const char *param_name,
                                              const uint8_t *sysex,
                                              size_t length, int *out_int,
                                              float *out_value1,
                                              float *out_value2) {
  size_t param_len = strlen(param_name);

  // Check if the SysEx message is long enough and contains the parameter name
  if (length > param_len && memcmp(sysex, param_name, param_len) == 0) {
    // Allocate a temporary buffer for the rest of the message
    char value_str[length - param_len + 1];

    // Copy the rest of the message into the buffer
    for (size_t i = param_len; i < length; i++) {
      value_str[i - param_len] = sysex[i];
    }

    // Null-terminate the string
    value_str[length - param_len] = '\0';

    // Tokenize the string to extract the integer and two float values
    char *token = strtok(value_str, "_");
    if (token == NULL) return false;

    // Parse the integer
    *out_int = strtol(token, NULL, 10);

    // Parse the first float
    token = strtok(NULL, "_");
    if (token == NULL) return false;
    *out_value1 = strtof(token, NULL);

    // Parse the second float
    token = strtok(NULL, "_");
    if (token == NULL) return false;
    *out_value2 = strtof(token, NULL);

    return true;
  }

  return false;
}

bool get_sysex_param_int_value(const char *param_name, const uint8_t *sysex,
                               size_t length, int *out_value) {
  size_t param_len = strlen(param_name);

  // Check if the SysEx message is long enough and contains the parameter name
  if (length > param_len && memcmp(sysex, param_name, param_len) == 0) {
    // Allocate a temporary buffer for the float part
    char value_str[length - param_len + 1];

    // Copy the float part into the buffer
    for (size_t i = param_len; i < length; i++) {
      value_str[i - param_len] = sysex[i];
    }

    // Null-terminate the string
    value_str[length - param_len] = '\0';

    // Convert the extracted string to a float and store it in out_value
    *out_value = atoi(value_str);
    return true;
  }

  // Return false if the parameter name is not found or the message is invalid
  return false;
}

void midi_sysex_callback(uint8_t *sysex, int length) {
#ifdef DEBUG_MIDI
  // build a string from the sysex buffer
  char sysex_str[length + 2 + 7];
  sysex_str[0] = 's';
  sysex_str[1] = 'y';
  sysex_str[2] = 's';
  sysex_str[3] = 'e';
  sysex_str[4] = 'x';
  sysex_str[5] = ':';
  sysex_str[6] = ' ';
  for (int j = 0; j < length; j++) {
    sysex_str[j + 7] = sysex[j];
  }
  sysex_str[length + 7] = '\n';
  send_buffer_as_sysex(sysex_str, length + 7 + 1);
#endif
  float val;
  float val2;
  int vali;
  // check if sysex starts with LN (lua new)
  if (sysex[0] == 'L' && (sysex[1] == 'A' || sysex[1] == 'N')) {
    // 36 byte chunks are sent L[A|N]<scene><output><32bytes>
    printf("LA%d\n", length);
    uint8_t scene = sysex[2] - '0';
    uint8_t output = sysex[3] - '0';
    Yoctocore_add_code(&yocto, scene, output, (char *)sysex + 4, length - 4,
                       sysex[1] == 'A');
  } else if (get_sysex_param_float_value("version", sysex, length, &val)) {
    printf("v1.0.0");
  } else if (get_sysex_param_float_value("diskmode", sysex, length, &val)) {
    sleep_ms(10);
    reset_usb_boot(0, 0);
  } else if (get_sysex_param_int_float_values("setvolt", sysex, length, &vali,
                                              &val)) {
    // voltset_<channel>_<volts>
    // voltage override
    uint8_t output = vali - 1;
    yocto.out[output].voltage_do_override = val >= -5.0f && val <= 10.0f;
    if (yocto.out[output].voltage_do_override) {
      yocto.out[output].voltage_override = val;
    }
  } else if (get_sysex_param_float_value("useraw", sysex, length, &val)) {
    for (uint8_t i = 0; i < 8; i++) {
      dac.use_raw[i] = val >= 0.5f;
    }
    if (val >= 0.5f) {
      printf("using raw\n");
    } else {
      printf("using calibrated\n");
    }
  } else if (get_sysex_param_int_and_two_float_values("cali", sysex, length,
                                                      &vali, &val, &val2)) {
    if (val == 0 && val2 == 0) {
      // get calibration
      Yoctocore_get_calibrations(&yocto);
      for (uint8_t i = 0; i < 8; i++) {
        printf("cali %d %f %f\n", i, yocto.out[i].voltage_calibration_slope,
               yocto.out[i].voltage_calibration_intercept);
      }
    } else {
      // set calibration
      if (Yoctocore_set_calibration(&yocto, vali, val, val2)) {
        dac.voltage_calibration_slope[vali] = val;
        dac.voltage_calibration_intercept[vali] = val2;
      }
    }
  } else {
    Yoctocore_process_sysex(&yocto, sysex);
    // clear the sysex buffer
  }
}

void midi_start() {
#ifdef DEBUG_MIDI
  printf("[midicallback] midi start\n");
#endif
}
void midi_continue() {
#ifdef DEBUG_MIDI
  printf("[midicallback] midi continue (starting)\n");
#endif
  midi_start();
}
void midi_stop() {
#ifdef DEBUG_MIDI
  printf("[midicallback] midi stop\n");
#endif
}

#endif