
#ifndef MCP4725_LIB
#define MCP4725_LIB 1

#include <stdio.h>

#include "hardware/i2c.h"

#define MCP4725_ADDRESS 0x62

typedef struct MCP4725 {
  i2c_inst_t *i2c;
  float voltage;
  uint8_t address;
} MCP4725;

void MCP4725_init(MCP4728 *self, i2c_inst_t *i2c, bool address_bit) {
  self->i2c = i2c;
  self->address = MCP4728_ADDRESS + address_bit;
}

#endif