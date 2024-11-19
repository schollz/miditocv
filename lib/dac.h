
#ifndef DAC_LIB
#define DAC_LIB 1

#include "mcp4728.h"

typedef struct DAC {
  MCP4728 mcp4728[2];
  float voltages[8];
  float voltages_last[8];
} DAC;

void DAC_init(DAC *self) {
  MCP4728_init(&self->mcp4728[0], i2c0, false, REFERENCE_5V);
  MCP4728_init(&self->mcp4728[1], i2c1, false, REFERENCE_5V);

  for (int i = 0; i < 8; i++) {
    self->voltages[i] = 0;
    self->voltages_last[i] = -1;
  }
}

void DAC_update(DAC *self) {
  bool mcp4728_changed[2] = {false, false};
  for (int i = 0; i < 2; i++) {
    // see if changed
    for (int j = 0; j < 4; j++) {
      if (self->voltages[j + i * 4] != self->voltages_last[j + i * 4]) {
        mcp4728_changed[i] = true;
        break;
      }
    }
    if (mcp4728_changed[i]) {
      MCP4728_update(&self->mcp4728[i]);
    }
  }
  for (int i = 0; i < 8; i++) {
    self->voltages_last[i] = self->voltages[i];
  }
}

void DAC_set_voltage(DAC *self, int channel, float voltage) {
  if (channel < 0 || channel >= 8) {
    return;
  }
#ifndef DEBUG_VOLTAGE_CALIBRATION
  // calibration
  voltage = (voltage - 0.126) / 0.988;
#endif
  // convert voltage from -5-10 to 0-5
  voltage = (voltage + 5.0f) / 3.0f;
  if (voltage < 0) {
    voltage = 0;
  } else if (voltage > REFERENCE_5V) {
    voltage = REFERENCE_5V;
  }
  voltage = REFERENCE_5V - voltage;
  self->voltages[channel] = voltage;
  if (channel < 4) {
    MCP4728_set_voltage(&self->mcp4728[0], channel, voltage);
  } else {
    MCP4728_set_voltage(&self->mcp4728[1], channel - 4, voltage);
  }
}

void DAC_set_voltage_update(DAC *self, int channel, float voltage) {
  DAC_set_voltage(self, channel, voltage);
  DAC_update(self);
}

#endif