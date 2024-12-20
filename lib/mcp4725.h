
#ifndef MCP4725_LIB
#define MCP4725_LIB 1

#include <stdio.h>

#include "hardware/i2c.h"

#define MCP4725_ADDRESS 0x62
#define MCP4725_CMD_WRITEDAC (0x40)

typedef struct MCP4725 {
  i2c_inst_t *i2c;
  float voltage;
  float voltage_reference;
  uint8_t address;
} MCP4725;

void MCP4725_init(MCP4725 *self, i2c_inst_t *i2c, bool address_bit,
                  float external_voltage) {
  self->i2c = i2c;
  self->address = MCP4725_ADDRESS + address_bit;
  self->voltage_reference = external_voltage;
}

void MCP4725_update(MCP4725 *self) {
  uint16_t value =
      (uint16_t)round(self->voltage * 4096.0 / self->voltage_reference);

  uint8_t packet[3];
  packet[0] = MCP4725_CMD_WRITEDAC;
  packet[1] = value / 16;
  packet[2] = (value % 16) << 4;

  uint8_t ret = i2c_write_blocking(self->i2c, self->address, packet, 3, false);
  if (ret == PICO_ERROR_GENERIC) {
    printf("Error setting mcp4725 voltage\n");
  }
}

void MCP4725_set_voltage(MCP4725 *self, float voltage) {
  self->voltage = voltage;
}

#endif