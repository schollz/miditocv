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
  float E;
} DataStruct;

void DataStruct_marshal(const DataStruct *data, uint8_t *buffer) {
  memcpy(buffer, data, sizeof(DataStruct));
}

void DataStruct_unmarshal(DataStruct *data, const uint8_t *buffer) {
  memcpy(data, buffer, sizeof(DataStruct));
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
  printf("DataStruct: %d %d %d %d %2.3f\n", data->A, data->B, data->C, data->D,
         data->E);

  data->A++;
  data->B++;
  data->C++;
  data->D++;
  data->E += 3.14;

  if (data->A > 122953999) {
    // reset all to 0
    data->A = 0;
    data->B = 0;
    data->C = 0;
    data->D = 0;
    data->E = 0.0;
  }

  // marshal data
  DataStruct_marshal(data, flash_data);
  pico_flash_write(flash_data);

  free(data);
}
