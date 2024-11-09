#include <stdint.h>
#include <stdio.h>

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

#define FLASH_TARGET_OFFSET (1792 * 1024)

typedef struct {
  int32_t A;
  int32_t B;
  int32_t C;
  int32_t D;
} DataStruct;

void DataStruct_marshal(DataStruct *data, uint8_t *buffer) {
  buffer[0] = (data->A >> 24) & 0xFF;
  buffer[1] = (data->A >> 16) & 0xFF;
  buffer[2] = (data->A >> 8) & 0xFF;
  buffer[3] = data->A & 0xFF;

  buffer[4] = (data->B >> 24) & 0xFF;
  buffer[5] = (data->B >> 16) & 0xFF;
  buffer[6] = (data->B >> 8) & 0xFF;
  buffer[7] = data->B & 0xFF;

  buffer[8] = (data->C >> 24) & 0xFF;
  buffer[9] = (data->C >> 16) & 0xFF;
  buffer[10] = (data->C >> 8) & 0xFF;
  buffer[11] = data->C & 0xFF;

  buffer[12] = (data->D >> 24) & 0xFF;
  buffer[13] = (data->D >> 16) & 0xFF;
  buffer[14] = (data->D >> 8) & 0xFF;
  buffer[15] = data->D & 0xFF;
}

void DataStruct_unmarshal(DataStruct *data, uint8_t *buffer) {
  data->A =
      (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
  data->B =
      (buffer[4] << 24) | (buffer[5] << 16) | (buffer[6] << 8) | buffer[7];
  data->C =
      (buffer[8] << 24) | (buffer[9] << 16) | (buffer[10] << 8) | buffer[11];
  data->D =
      (buffer[12] << 24) | (buffer[13] << 16) | (buffer[14] << 8) | buffer[15];
}

void pico_flash_read(uint8_t *buffer, size_t len) {
  const uint8_t *flash_target_contents =
      (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET);
  for (size_t i = 0; i < len; ++i) {
    buffer[i] = flash_target_contents[i];
  }
}

void pico_flash_write(uint8_t *buffer) {
  uint32_t interrupts = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET, FLASH_SECTOR_SIZE);
  flash_range_program(FLASH_TARGET_OFFSET, buffer, FLASH_PAGE_SIZE);
  restore_interrupts(interrupts);
}

void flash_mem_test() {
  uint8_t flash_data[FLASH_PAGE_SIZE];
  pico_flash_read(flash_data, FLASH_PAGE_SIZE);

  DataStruct *data = malloc(sizeof(DataStruct));
  DataStruct_unmarshal(data, flash_data);
  printf("DataStruct: %d %d %d %d\n", data->A, data->B, data->C, data->D);

  data->A++;
  data->B++;
  data->C++;
  data->D++;

  if (data->A > 122953999) {
    // reset all to 0
    data->A = 0;
    data->B = 0;
    data->C = 0;
    data->D = 0;
  }

  // marshal data
  DataStruct_marshal(data, flash_data);
  pico_flash_write(flash_data);

  free(data);
}
