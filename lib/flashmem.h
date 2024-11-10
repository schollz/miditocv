#include <stdint.h>
#include <stdio.h>

#include "hardware/flash.h"
#include "hardware/sync.h"
#include "pico/stdlib.h"

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

void pico_flash_read(uint8_t *buffer, size_t len, size_t offset) {
  const uint8_t *flash_target_contents =
      (const uint8_t *)(XIP_BASE + FLASH_TARGET_OFFSET +
                        (offset * FLASH_PAGE_SIZE));
  for (size_t i = 0; i < len; ++i) {
    buffer[i] = flash_target_contents[i];
  }
}

void pico_flash_erase(uint8_t offset) {
  uint32_t interrupts = save_and_disable_interrupts();
  flash_range_erase(FLASH_TARGET_OFFSET + (offset * FLASH_SECTOR_SIZE),
                    FLASH_SECTOR_SIZE);
  restore_interrupts(interrupts);
}

void pico_flash_write(uint8_t *buffer, size_t offset) {
  uint32_t interrupts = save_and_disable_interrupts();
  flash_range_program(FLASH_TARGET_OFFSET + offset * FLASH_PAGE_SIZE, buffer,
                      FLASH_PAGE_SIZE);
  restore_interrupts(interrupts);
}

void flash_mem_test() {
  uint8_t flash_data[FLASH_PAGE_SIZE];
  // make an array of 4 DataStructs
  DataStruct *data[4];
  for (int i = 0; i < 4; i++) {
    data[i] = malloc(sizeof(DataStruct));
  }

  for (int i = 0; i < 4; i++) {
    pico_flash_read(flash_data, FLASH_PAGE_SIZE, i);
    DataStruct_unmarshal(data[i], flash_data + i * sizeof(DataStruct));
    printf("DataStruct[%d]: %d %d %d %d %2.3f\n", i, data[i]->A, data[i]->B,
           data[i]->C, data[i]->D, data[i]->E);
    if (data[i]->A > 122953999 || data[i]->E < -1000) {
      // reset all to 0
      data[i]->A = 0;
      data[i]->B = 0;
      data[i]->C = 0;
      data[i]->D = 0;
      data[i]->E = 0.0;
    }
    data[i]->A += 1;
    data[i]->B += 2;
    data[i]->C += 3;
    data[i]->D += 4;
    data[i]->E += 0.1;
  }

  // erase sector
  pico_flash_erase(0);

  // write data
  for (int i = 0; i < 4; i++) {
    DataStruct_marshal(data[i], flash_data + i * sizeof(DataStruct));
    pico_flash_write(flash_data, i);
  }

  for (int i = 0; i < 4; i++) {
    free(data[i]);
  }
}
