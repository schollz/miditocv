#ifndef CIRCULAR_BUFFER_H
#define CIRCULAR_BUFFER_H
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define BUFFER_SIZE (441 * 10)

typedef enum { BUFFER1, BUFFER2, BUFFER3 } BufferState;

int16_t buffer1[BUFFER_SIZE];
int16_t buffer2[BUFFER_SIZE];
int16_t buffer3[BUFFER_SIZE];
int16_t buffer4[BUFFER_SIZE];
int16_t buffer5[BUFFER_SIZE];
int16_t buffer6[BUFFER_SIZE];

// Function to get a pointer to the active buffer
int16_t *getActiveBuffer(int id, BufferState activeBuffer) {
  switch (id) {
    case 1:
      if (activeBuffer == BUFFER1) {
        return buffer4;
      } else if (activeBuffer == BUFFER2) {
        return buffer5;
      } else {
        return buffer6;
      }
    default:
      if (activeBuffer == BUFFER1) {
        return buffer1;
      } else if (activeBuffer == BUFFER2) {
        return buffer2;
      } else {
        return buffer3;
      }
  }
}

typedef struct CircularBuffer {
  uint8_t id;
  BufferState buffer_read;
  BufferState buffer_read_last;
  uint32_t head_read;
  bool write_ready;
} CircularBuffer;

CircularBuffer *CircularBuffer_malloc(uint8_t id) {
  CircularBuffer *cb = (CircularBuffer *)malloc(sizeof(CircularBuffer));
  if (cb == NULL) {
    return NULL;
  }
  cb->id = id;
  cb->head_read = 0;
  cb->buffer_read = BUFFER1;
  cb->write_ready = false;
  for (uint32_t i = 0; i < BUFFER_SIZE; i++) {
    buffer1[i] = 0;
    buffer2[i] = 0;
    buffer3[i] = 0;
    buffer4[i] = 0;
    buffer5[i] = 0;
    buffer6[i] = 0;
  }
  return cb;
}

void CircularBuffer_free(CircularBuffer *cb) {
  if (cb == NULL) {
    return;
  }
  free(cb);
}

bool CircularBuffer_write_ready(CircularBuffer *cb) { return cb->write_ready; }

uint32_t CircularBuffer_size() { return BUFFER_SIZE; }

void CircularBuffer_write(CircularBuffer *cb, int16_t *data) {
  // swap buffers
  int16_t *buffer = getActiveBuffer(cb->id, cb->buffer_read_last);
  memcpy(buffer, data, BUFFER_SIZE * sizeof(int16_t));
}

bool CircularBuffer_read(CircularBuffer *cb, int16_t *data, uint32_t size) {
  if (cb->head_read + size <= BUFFER_SIZE) {
    int16_t *buffer = getActiveBuffer(cb->id, cb->buffer_read);
    memcpy(data, buffer + cb->head_read, size * sizeof(int16_t));
  }
  cb->head_read += size;
  if (cb->head_read >= BUFFER_SIZE) {
    cb->head_read = 0;
    BufferState current = cb->buffer_read;
    if (current == BUFFER1) {
      cb->buffer_read = BUFFER2;
    } else if (current == BUFFER2) {
      cb->buffer_read = BUFFER3;
    } else {
      cb->buffer_read = BUFFER1;
    }
    cb->buffer_read_last = current;
    return true;
  }
  return false;
}

#endif