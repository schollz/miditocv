#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../circular_buffer.h"

int main() {
  CircularBuffer *cb = CircularBuffer_malloc(10);
  if (cb == NULL) {
    printf("Failed to allocate CircularBuffer\n");
    return 1;
  }
#define WRITE 5
#define READ 3
  int16_t data_write[WRITE] = {1, 2, 3, 4, 5};
  int16_t data_read[READ];
  CircularBuffer_write(cb, data_write, WRITE);
  CircularBuffer_read(cb, data_read, READ);
  for (uint32_t i = 0; i < READ; i++) {
    printf("%d ", data_read[i]);
  }
  printf("\nUnread size: %d\n", CircularBuffer_get_unread_size(cb));
  CircularBuffer_write(cb, data_write, WRITE);
  CircularBuffer_read(cb, data_read, READ);
  for (uint32_t i = 0; i < READ; i++) {
    printf("%d ", data_read[i]);
  }
  CircularBuffer_read(cb, data_read, READ);
  for (uint32_t i = 0; i < READ; i++) {
    printf("%d ", data_read[i]);
  }
  CircularBuffer_read(cb, data_read, READ);
  for (uint32_t i = 0; i < READ; i++) {
    printf("%d ", data_read[i]);
  }
  CircularBuffer_read(cb, data_read, READ);
  for (uint32_t i = 0; i < READ; i++) {
    printf("%d ", data_read[i]);
  }
  printf("\nUnread size: %d\n", CircularBuffer_get_unread_size(cb));
  CircularBuffer_write(cb, data_write, WRITE);
  CircularBuffer_read(cb, data_read, READ);
  for (uint32_t i = 0; i < READ; i++) {
    printf("%d ", data_read[i]);
  }
  printf("\nUnread size: %d\n", CircularBuffer_get_unread_size(cb));
  CircularBuffer_free(cb);
  return 0;
}
