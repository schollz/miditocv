#ifndef LIB_MIDI_UART_H
#define LIB_MIDI_UART_H 1

#include <stdint.h>
#include <stdio.h>

// MIDI message types
enum {
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_ON = 0x90,
  MIDI_AFTERTOUCH = 0xA0,
  MIDI_CONTROL_CHANGE = 0xB0,
  MIDI_PROGRAM_CHANGE = 0xC0,
  MIDI_CHANNEL_PRESSURE = 0xD0,
  MIDI_PITCH_BEND = 0xE0,
  MIDI_SYSEX = 0xF0,
  MIDI_SYSEX_END = 0xF7,
  MIDI_TIMING_CLOCK = 0xF8,
  MIDI_ACTIVE_SENSE = 0xFE,
  MIDI_START = 0xFA,
  MIDI_CONTINUE = 0xFB,
  MIDI_STOP = 0xFC,
};

// State of the MIDI processor
typedef struct MidiUart {
  uint8_t status;      // Running status byte
  uint8_t data[2];     // Temporary storage for data bytes
  uint8_t data_count;  // Number of data bytes received
} MidiUart;

// Initialize the MIDI processor
void MidiUart_init(MidiUart *self) {
  self->status = 0;
  self->data[0] = 0;
  self->data[1] = 0;
  self->data_count = 0;
}

// Process a single byte of MIDI data
void MidiUart_process(MidiUart *self, uint8_t byte) {
  if (byte >= 0xF8) {
    // Real-time messages (do not affect running status)
    printf("Real-time message: 0x%02X\n", byte);
    return;
  }

  if (byte >= 0x80) {
    // Status byte
    self->status = byte;
    self->data_count = 0;  // Reset data count for a new message
    printf("Status byte received: 0x%02X\n", self->status);
    return;
  }

  if (self->status == 0) {
    // Ignore data byte if no running status is set
    printf("Ignoring data byte 0x%02X (no running status)\n", byte);
    return;
  }

  // Handle data bytes
  self->data[self->data_count++] = byte;

  // Determine the number of data bytes expected
  uint8_t expected_data_bytes = 0;
  if ((self->status & 0xF0) == MIDI_NOTE_OFF ||
      (self->status & 0xF0) == MIDI_NOTE_ON ||
      (self->status & 0xF0) == MIDI_AFTERTOUCH ||
      (self->status & 0xF0) == MIDI_CONTROL_CHANGE ||
      (self->status & 0xF0) == MIDI_PITCH_BEND) {
    expected_data_bytes = 2;
  } else if ((self->status & 0xF0) == MIDI_PROGRAM_CHANGE ||
             (self->status & 0xF0) == MIDI_CHANNEL_PRESSURE) {
    expected_data_bytes = 1;
  }

  // If the required number of data bytes has been received, process the message
  if (self->data_count == expected_data_bytes) {
    uint8_t channel = self->status & 0x0F;  // Extract the channel
    uint8_t command = self->status & 0xF0;  // Extract the command type

    switch (command) {
      case MIDI_NOTE_OFF:
        printf("Note Off - Channel: %d, Note: %d, Velocity: %d\n", channel,
               self->data[0], self->data[1]);
        break;
      case MIDI_NOTE_ON:
        if (self->data[1] == 0) {
          printf("Note Off (Note On with Velocity 0) - Channel: %d, Note: %d\n",
                 channel, self->data[0]);
        } else {
          printf("Note On - Channel: %d, Note: %d, Velocity: %d\n", channel,
                 self->data[0], self->data[1]);
        }
        break;
      case MIDI_CONTROL_CHANGE:
        printf("Control Change - Channel: %d, Controller: %d, Value: %d\n",
               channel, self->data[0], self->data[1]);
        break;
      case MIDI_PROGRAM_CHANGE:
        printf("Program Change - Channel: %d, Program: %d\n", channel,
               self->data[0]);
        break;
      case MIDI_CHANNEL_PRESSURE:
        printf("Channel Pressure - Channel: %d, Pressure: %d\n", channel,
               self->data[0]);
        break;
      case MIDI_PITCH_BEND: {
        int16_t pitch = (self->data[1] << 7) | self->data[0];
        printf("Pitch Bend - Channel: %d, Value: %d\n", channel, pitch);
      } break;
      default:
        printf("Unknown MIDI Command: 0x%02X\n", command);
        break;
    }

    // Reset the data count for the next message
    self->data_count = 0;
  }
}

#endif
