// MIDI code adapted from Jacob Vosmaer
// MIT License
//
// Copyright (c) 2020 andrewikenberry
// Copyright (c) 2023 Jacob Vosmaer
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef LIB_MIDI_UART_H
#define LIB_MIDI_UART_H 1

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum {
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_ON = 0x90,
  MIDI_AFTERTOUCH = 0xa0,
  MIDI_CONTROL_CHANGE = 0xb0,
  MIDI_PROGRAM_CHANGE = 0xc0,
  MIDI_CHANNEL_PRESSURE = 0xd0,
  MIDI_PITCH_BEND = 0xe0,
  MIDI_SYSEX = 0xf0,
  MIDI_SYSEX_END = 0xf7,
  MIDI_TIMING_CLOCK = 0xf8,
  MIDI_ACTIVE_SENSE = 0xfe,
  MIDI_START = 0xfa,
  MIDI_CONTINUE = 0xfb,
  MIDI_STOP = 0xfc,
};

typedef struct MidiUart {
  uint8_t status;
  uint8_t previous;
  uint32_t last_time;
} MidiUart;

void MidiUart_init(MidiUart *self) {
  self->status = 0;
  self->previous = 0;
  self->last_time = 0;
}

uint8_t MidiUart_reverse_uint8_t(uint8_t b) {
  b = (b & 0xF0) >> 4 | (b & 0x0F) << 4;
  b = (b & 0xCC) >> 2 | (b & 0x33) << 2;
  b = (b & 0xAA) >> 1 | (b & 0x55) << 1;
  return b;
}

typedef struct midi_message {
  uint8_t status;
  uint8_t data[2];
} midi_message;

void MidiUart_process(MidiUart *self, uint8_t b) {
  if (b != 0xf8 && b != 0xfe) {
    printf("[onewiremidi] received: %02x\n", b);
  }

  enum { DATA0_PRESENT = 0x80 };
  midi_message msg = {0};

  if (b >= 0xf8) {
    msg.status = b;
    if (msg.status == MIDI_TIMING_CLOCK) {
    } else if (msg.status == MIDI_START) {
    } else if (b == MIDI_CONTINUE) {
    } else if (b == MIDI_STOP) {
    }
  } else if (b >= 0xf4) {
    msg.status = b;
    self->status = 0;
  } else if (b >= 0x80) {
    self->status = b;
    self->previous = b == 0xf0 ? b : 0;
  } else if ((self->status >= 0xc0 && self->status < 0xe0) ||
             (self->status >= 0xf0 && self->status != 0xf2)) {
    msg.status = self->status;
    msg.data[0] = b;
    msg.data[1] = self->previous;
    self->previous = 0;
  } else if (self->status && self->previous & DATA0_PRESENT) {
    msg.status = self->status;
    msg.data[0] = self->previous ^ DATA0_PRESENT;
    msg.data[1] = b;
    self->previous = 0;
    if (msg.status == MIDI_NOTE_ON) {
      printf("[midiuart] note on: %d %d\n", msg.data[0], msg.data[1]);
    } else if (msg.status == MIDI_NOTE_OFF) {
      printf("[midiuart] note off\n");
    }
  } else {
    self->previous = b | DATA0_PRESENT;
  }

  return;
}

#endif