
#ifndef DAC_LIB
#define DAC_LIB 1

#include "mcp4728.h"

typedef struct DAC {
  MCP4728 *mcp4728[2];
  float voltages[8];
  float voltages_last[8];
} DAC;

void DAC_free(DAC *self) {
  MCP4728_free(self->mcp4728[0]);
  MCP4728_free(self->mcp4728[1]);
  free(self);
}

DAC *DAC_malloc() {
  DAC *dac = (DAC *)malloc(sizeof(DAC));
  dac->mcp4728[0] = MCP4728_malloc(i2c0, true, 0);
  dac->mcp4728[1] = MCP4728_malloc(i2c1, true, 0);

  for (int i = 0; i < 8; i++) {
    dac->voltages[i] = 0;
  }
  return dac;
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
      MCP4728_update(self->mcp4728[i]);
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
  self->voltages[channel] = voltage;
  if (channel < 4) {
    MCP4728_set_voltage(self->mcp4728[0], channel, voltage);
  } else {
    MCP4728_set_voltage(self->mcp4728[1], channel - 4, voltage);
  }
}

void DAC_set_voltage_update(DAC *self, int channel, float voltage) {
  DAC_set_voltage(self, channel, voltage);
  DAC_update(self);
}

#endif