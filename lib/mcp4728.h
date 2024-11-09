
#ifndef MCP4728_LIB
#define MCP4728_LIB 1

#include <stdio.h>

#include "hardware/i2c.h"

#define MCP4728_ADDRESS 0x67

typedef struct MCP4728 {
  i2c_inst_t *i2c;
  float voltage[4];
  bool use_internal_ref;
  float voltage_reference;
  uint8_t address;
} MCP4728;

void MCP4728_write_address(MCP4728 *self, uint8_t address, uint8_t data) {
  uint8_t buf[2] = {address, data};
  i2c_write_blocking(self->i2c, self->address, buf, 2, false);
}

void MCP4728_update(MCP4728 *self) {
  // page 38 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
  uint8_t data[8];
  for (int i = 0; i < 4; i++) {
    // printf("voltage[%d] = %f\n", i, self->voltage[i]);
    uint16_t value =
        (uint16_t)round(self->voltage[i] * 4095.0 / self->voltage_reference);
    data[i * 2] = 0b00000000 | (value >> 8);
    data[i * 2 + 1] = value & 0xff;
  }
  uint8_t ret = i2c_write_blocking(self->i2c, self->address, data, 8, false);
  if (ret == PICO_ERROR_GENERIC) {
    printf("Error writing to MCP4728\n");
  }
}

void MCP4728_free(MCP4728 *self) { free(self); }

MCP4728 *MCP4728_malloc(i2c_inst_t *i2c, bool use_internal_ref,
                        float external_voltage) {
  MCP4728 *self = (MCP4728 *)malloc(sizeof(MCP4728));
  self->i2c = i2c;
  self->address = MCP4728_ADDRESS;
  self->use_internal_ref = use_internal_ref;
  self->voltage_reference = external_voltage;
  if (use_internal_ref) {
    // set to Internal Reference (2.048V)
    // page 43 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
    uint8_t data = 0b10001111;
    uint8_t ret = i2c_write_blocking(self->i2c, self->address, &data, 1, false);
    if (ret == PICO_ERROR_GENERIC) {
      printf("Error setting internal reference\n");
    }
    sleep_us(100);
    // set to Gain of 2 so Vref = 4.096V
    // page 44 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
    data = 0b11001111;
    ret = i2c_write_blocking(self->i2c, self->address, &data, 1, false);
    if (ret == PICO_ERROR_GENERIC) {
      printf("Error setting gain\n");
    }
    sleep_us(100);
    self->voltage_reference = 4.096;
  } else {
    // set to External Reference
    // page 43 http://ww1.microchip.com/downloads/en/devicedoc/22187e.pdf
    uint8_t data = 0b10000000;
    uint8_t ret = i2c_write_blocking(self->i2c, self->address, &data, 1, false);
    if (ret == PICO_ERROR_GENERIC) {
      printf("Error setting external reference\n");
    }
  }
  for (int i = 0; i < 4; i++) {
    self->voltage[i] = 0;
  }

  MCP4728_update(self);
  return self;
}

void MCP4728_set_voltage(MCP4728 *self, uint8_t ch, float voltage) {
  self->voltage[ch] = voltage;
}

#endif