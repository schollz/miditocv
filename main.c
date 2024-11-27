#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/flash.h"
#include "hardware/i2c.h"
#include "hardware/irq.h"
#include "hardware/pio.h"
#include "hardware/pll.h"
#include "hardware/rtc.h"
#include "hardware/structs/clocks.h"
#include "hardware/sync.h"
#include "hardware/uart.h"
#include "hardware/watchdog.h"
#include "pico/binary_info.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/types.h"

bool usb_midi_present = false;
#ifdef INCLUDE_MIDI
#include "bsp/board.h"
#include "tusb.h"
//
#include "lib/midi_comm.h"
#endif

// utility functions
#define util_clamp(x, a, b) ((x) > (b) ? (b) : ((x) < (a) ? (a) : (x)))

#define linlin(x, xmin, xmax, ymin, ymax)                                 \
  util_clamp((ymin + (x - xmin) * (ymax - ymin) / (xmax - xmin)), (ymin), \
             (ymax))

#define BLOCKS_PER_SECOND SAMPLE_RATE / SAMPLES_PER_BUFFER
static const uint32_t PIN_DCDC_PSM_CTRL = 23;
#define DURATION_HOLD 500
#define DURATION_HOLD_LONG 1250
#define FLASH_TARGET_OFFSET (5 * 256 * 1024)

//
#include "ff.h" /* Obtains integer types */
//
#include "diskio.h" /* Declarations of disk functions */
//
#include "f_util.h"
#include "hw_config.h"
#include "my_debug.h"
#include "sd_card.h"
//
//
#include "lib/WS2812.h"
#include "lib/adsr.h"
#include "lib/dac.h"
#include "lib/filterexp.h"
#include "lib/knob_change.h"
#include "lib/libmidi.h"
#include "lib/mcp3208.h"
#include "lib/memusage.h"
#include "lib/pcg_basic.h"
#include "lib/random.h"
#include "lib/scales.h"
#include "lib/sdcard.h"
#include "lib/simpletimer.h"
#include "lib/spectra.h"
#include "lib/spiral.h"
#include "lib/yoctocore.h"

Yoctocore yocto;
DAC dac;
WS2812 ws2812;
SimpleTimer pool_timer[16];
KnobChange pool_knobs[8];
MCP3208 mcp3208;
const uint8_t button_num = 9;
const uint8_t button_pins[9] = {1, 8, 20, 21, 22, 26, 27, 28, 29};
uint8_t button_values[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

#define UART_ID uart1
#define BAUD_RATE 31250
#define DATA_BITS 8
#define STOP_BITS 1
#define PARITY UART_PARITY_NONE

#ifdef INCLUDE_MIDI
#include "lib/midi_comm.h"
#include "lib/midicallback.h"
#endif

void setup_uart() {
  // Set up our UART with a basic baud rate.
  uart_init(UART_ID, 2400);

  // Set the TX and RX pins by using the function select on the GPIO
  // Set datasheet for more information on function select
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Actually, we want a different speed
  // The call will return the actual baud rate selected, which will be as close
  // as possible to that requested
  int __unused actual = uart_set_baudrate(UART_ID, BAUD_RATE);

  // // Set UART flow control CTS/RTS, we don't want these, so turn them off
  uart_set_hw_flow(UART_ID, false, false);

  // Set our data format
  uart_set_format(UART_ID, DATA_BITS, STOP_BITS, PARITY);

  // // Turn off FIFO's - we want to do this character by character
  uart_set_fifo_enabled(UART_ID, true);
}

void timer_callback_outputs(bool on, int user_data) {
  printf("[timer_callback_outputs]: %d %d\n", on, user_data);
  Out *out = &yocto.out[user_data];
  Config *config = &yocto.config[yocto.i][user_data];
  if (on) {
    out->voltage_current = config->max_voltage;
  } else {
    out->voltage_current = config->min_voltage;
  }
}

void timer_callback_sample_knob(bool on, int user_data) {
  for (uint8_t i = 0; i < 8; i++) {
    int16_t val_changed =
        KnobChange_update(&pool_knobs[i], MCP3208_read(&mcp3208, i, false));
    if (val_changed != -1) {
      printf("Knob %d: %d\n", i, val_changed);
    }
  }
}

uint8_t gammaCorrectUint8_t(float value) {
  return roundf(255.0f * powf(value, 0.8f));
}

const uint8_t const_colors[8][3] = {
    {160, 160, 160},  // White
    {255, 0, 0},      // Red
    {255, 74, 0},     // Orange
    {250, 175, 0},    // Yellow
    {0, 255, 0},      // Green
    {0, 255, 255},    // Cyan
    {0, 0, 244},      // Blue
    {97, 0, 97},      // Violet
};

void midi_event_note_on(char chan, char data1, char data2) {
  printf("midi_event_note_on: %d %d %d\n", chan, data1, data2);
}

void midi_event_note_off(char chan, char data1, char data2) {
  printf("midi_event_note_off: %d %d %d\n", chan, data1, data2);
}

void timer_callback_ws2812(bool on, int user_data) {
  for (uint8_t i = 0; i < 8; i++) {
    // uint8_t jj =
    //     roundf(linlin(yocto.out[i].voltage_current, -5.0, 10.0, 0, 7.4));
    // for (uint8_t j = 0; j < 8; j++) {
    //   WS2812_fill(&ws2812, j, const_colors[jj][0], const_colors[jj][1],
    //               const_colors[jj][2]);
    // }
    // WS2812_show(&ws2812);
    // return;
    // float x = linlin(yocto.out[i].voltage_current, -5.0, 10.0, 0.0, 1.0);
    // uint8_t r, g, b;
    // RGB_Spectra_ToUint8(x, &r, &g, &b);
    // WS2812_fill(&ws2812, i, r, g, b);
    // if (i == 0) {
    //   printf("RGB: %d %d %d\n", r, g, b);
    // }
    // for (uint8_t j = 1; j < 8; j++) {
    //   WS2812_fill(&ws2812, j, r, g, b);
    // }
    // WS2812_show(&ws2812);
    // return;
    if (yocto.out[i].voltage_current < 0) {
      // 0 to -5V goes 0 -> blue with gamma correction
      float t = linlin(yocto.out[i].voltage_current, -5.0, 0.0, 0.0, 1.0);
      uint8_t blue = gammaCorrectUint8_t(t);  // Apply gamma to red intensity
      WS2812_fill(&ws2812, i, 0, 0, 255 - blue);
    } else if (yocto.out[i].voltage_current == 0) {
      // Voltage at 0V means off
      WS2812_fill(&ws2812, i, 0, 0, 0);
    } else if (yocto.out[i].voltage_current <= 5.0) {
      // 0 to 5V goes 0 -> green with gamma correction
      float t = linlin(yocto.out[i].voltage_current, 0.0, 5.0, 0.0, 1.0);
      uint8_t green = gammaCorrectUint8_t(t);  // Apply gamma to green intensity
      WS2812_fill(&ws2812, i, 0, green, 0);
    } else {
      // 5 to 10V goes green -> red with constant perceived brightness
      uint8_t brightness = 255;  // Maximum brightness level
      float t = linlin(yocto.out[i].voltage_current, 5.0, 10.0, 0.0, 1.0);

      // Calculate raw values for red and green
      float red_raw = t;
      float green_raw = 1.0 - t;

      // Apply gamma correction
      uint8_t red = gammaCorrectUint8_t(red_raw);
      uint8_t green = gammaCorrectUint8_t(green_raw);

      // Normalize to constant brightness
      float total = red + green;
      red = roundf((red / total) * brightness);
      green = roundf((green / total) * brightness);

      WS2812_fill(&ws2812, i, red, green,
                  0);  // Set LED with gamma-corrected brightness
    }
  }
  for (uint8_t i = 8; i < 16; i++) {
    WS2812_fill(&ws2812, i, 255, 255, 255);
  }
  WS2812_show(&ws2812);
}

void timer_callback_print_memory_usage(bool on, int user_data) {
  print_memory_usage();
}

void timer_callback_update_voltage(bool on, int user_data) {
  // update the DAC
  for (uint8_t i = 0; i < 8; i++) {
    DAC_set_voltage(&dac, i, yocto.out[i].voltage_current);
  }
  DAC_update(&dac);
}

void midi_note_off(int channel, int note) {
  uint32_t ct = to_ms_since_boot(get_absolute_time());
  channel++;  // 1-indexed
              // #ifdef DEBUG_MIDI
  printf("ch=%d note_off=%d\n", channel, note);
  // #endif
  bool outs_with_note_change[8] = {false, false, false, false,
                                   false, false, false, false};
  // check if any outputs are set to midi pitch
  for (uint8_t i = 0; i < 8; i++) {
    Config *config = &yocto.config[yocto.i][i];
    Out *out = &yocto.out[i];
    if (config->mode == MODE_PITCH &&
        (config->midi_channel == channel || config->midi_channel == 0) &&
        out->note_on.note == note) {
      // set the voltage
      out->note_on.note = 0;
      out->note_on.time_on = 0;
      outs_with_note_change[i] = true;
      printf("[out%d] note_off %d\n", i + 1, note);
    }
  }
  // find any linked outputs and activate the envelope
  for (uint8_t i = 0; i < 8; i++) {
    Config *config = &yocto.config[yocto.i][i];
    Out *out = &yocto.out[i];
    if (config->mode == MODE_ENVELOPE && config->linked_to > 0) {
      if (outs_with_note_change[config->linked_to - 1]) {
        // trigger the envelope
        printf("[out%d] env_ff linked to out%d\n", i + 1, config->linked_to);
        ADSR_gate(&out->adsr, 0, ct);
      }
    }
  }
}

void midi_note_on(int channel, int note, int velocity) {
  uint32_t ct = to_ms_since_boot(get_absolute_time());
  channel++;  // 1-indexed
  // #ifdef DEBUG_MIDI
  //   printf("ch=%d note_on=%d vel=%d\n", channel, note, velocity);
  // #endif
  // check if any outputs are set to midi pitch
  bool outs_with_note_change[8] = {false, false, false, false,
                                   false, false, false, false};
  for (uint8_t i = 0; i < 8; i++) {
    Config *config = &yocto.config[yocto.i][i];
    Out *out = &yocto.out[i];
    if (config->mode == MODE_PITCH &&
        (config->midi_channel == channel || config->midi_channel == 0) &&
        (out->note_on.time_on == 0 ||
         (ct - out->note_on.time_on) > MAX_NOTE_HOLD_TIME_MS) &&
        random_integer_in_range(0, 99) < config->probability) {
      // set the voltage for the pitch
      out->note_on.note = note;
      out->note_on.time_on = ct;
      out->voltage_set =
          (float)(note - config->root_note) * config->v_oct / 12.0f +
          config->min_voltage;
      outs_with_note_change[i] = true;
#ifdef DEBUG_MIDI
      printf("[out%d] %d %d %f %f to %f\n", i + 1, note, config->root_note,
             config->v_oct, config->min_voltage, out->voltage_set);
#endif
    }
  }
  // find any linked outputs and activate the envelope
  for (uint8_t i = 0; i < 8; i++) {
    Config *config = &yocto.config[yocto.i][i];
    Out *out = &yocto.out[i];
    if (config->mode == MODE_ENVELOPE && config->linked_to > 0) {
      if (outs_with_note_change[config->linked_to - 1]) {
        // trigger the envelope
        printf("[out%d] env_on linked to out%d\n", i + 1, config->linked_to);
        ADSR_gate(&out->adsr, 1, ct);
      }
    }
  }
}

int main() {
  // Set PLL_USB 96MHz
  const uint32_t main_line = 96;
  pll_init(pll_usb, 1, main_line * 16 * MHZ, 4, 4);
  clock_configure(clk_usb, 0, CLOCKS_CLK_USB_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                  main_line * MHZ, main_line / 2 * MHZ);
  // Change clk_sys to be 96MHz.
  clock_configure(clk_sys, CLOCKS_CLK_SYS_CTRL_SRC_VALUE_CLKSRC_CLK_SYS_AUX,
                  CLOCKS_CLK_SYS_CTRL_AUXSRC_VALUE_CLKSRC_PLL_USB,
                  main_line * MHZ, main_line * MHZ);
  // CLK peri is clocked from clk_sys so need to change clk_peri's freq
  clock_configure(clk_peri, 0, CLOCKS_CLK_PERI_CTRL_AUXSRC_VALUE_CLK_SYS,
                  main_line * MHZ, main_line * MHZ);
  // Reinit uart now that clk_peri has changed
  stdio_init_all();

#ifdef DO_OVERCLOCK
  set_sys_clock_khz(225000, true);
#else
  set_sys_clock_khz(125000, true);
#endif
  sleep_ms(10);

  // DCDC PSM control
  // 0: PFM mode (best efficiency)
  // 1: PWM mode (improved ripple)
  gpio_init(PIN_DCDC_PSM_CTRL);
  gpio_set_dir(PIN_DCDC_PSM_CTRL, GPIO_OUT);
  gpio_put(PIN_DCDC_PSM_CTRL, 1);  // PWM mode for less Audio noise

  // setup i2c
  i2c_init(i2c0, 50 * 1000);
  gpio_set_function(I2C0_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C0_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C0_SDA_PIN);
  gpio_pull_up(I2C0_SCL_PIN);
  i2c_init(i2c1, 50 * 1000);
  gpio_set_function(I2C1_SDA_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C1_SCL_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C1_SDA_PIN);
  gpio_pull_up(I2C1_SCL_PIN);

#ifdef INCLUDE_MIDI
  // setup midi
  tusb_init();
#endif

  // setup midi external
  setup_uart();
  midi_init();
  midi_register_event_handler(EVT_CHAN_NOTE_ON, midi_event_note_on);
  midi_register_event_handler(EVT_CHAN_NOTE_OFF, midi_event_note_off);

  // // // load the Scene data
  // Scene_load_data();

  // Implicitly called by disk_initialize,
  // but called here to set up the GPIOs
  // before enabling the card detect interrupt:
  sd_init_driver();

  // initialize random library
  random_initialize();

  // initialize knobs
  for (uint8_t i = 0; i < 8; i++) {
    KnobChange_init(&pool_knobs[i], 3);
  }

  // setup buttons
  for (uint8_t i = 0; i < button_num; i++) {
    gpio_init(button_pins[i]);
    gpio_set_dir(button_pins[i], GPIO_IN);
    gpio_pull_up(button_pins[i]);
  }

  // initialize MCP3208
  MCP3208_init(&mcp3208, spi0, PIN_SPI_CSN, PIN_SPI_CLK, PIN_SPI_RX,
               PIN_SPI_TX);

  // initialize WS2812
  WS2812_init(&ws2812, WS2812_PIN, pio0, WS2812_SM, 16);
  WS2812_set_brightness(&ws2812, 30);
  for (uint8_t i = 0; i < 16; i++) {
    WS2812_fill(&ws2812, i, 255, 0, 255);
  }
  WS2812_show(&ws2812);

  // initialize SD card
  // sleep_ms(1000);
  printf("[main]: initializing sd card\n");
  for (uint8_t i = SDCARD_CMD_GPIO - 1; i < SDCARD_D0_GPIO + 5; i++) {
    gpio_pull_up(i);
  }
  if (!run_mount()) {
    sleep_ms(1000);
    printf("[main]: failed to mount sd card\n");
  } else {
    // big_file_test("test.bin", 2, 0);  // perform read/write test
  }

  // initialize dac
  DAC_init(&dac);
#ifdef DEBUG_VOLTAGE_CALIBRATION
  sleep_ms(5000);
  printf("DAC calibration\n");
  for (int8_t volts = -5; volts <= 10; volts++) {
    for (uint8_t i = 0; i < 8; i++) {
      DAC_set_voltage(&dac, i, volts);
    }
    DAC_update(&dac);
    printf("calibration at %d\n", volts);
    sleep_ms(5000);
  }
#endif
  for (uint8_t i = 0; i < 8; i++) {
    DAC_set_voltage(&dac, i, 0);
  }
  DAC_update(&dac);

  // initialize the yoctocore
  Yoctocore_init(&yocto);
  // load the yoctocore data
  uint64_t start_time = time_us_64();
  sleep_ms(1000);
  if (Yoctocore_load(&yocto)) {
    printf("loaded data in %lld us\n", time_us_64() - start_time);
  } else {
    printf("failed to load data\n");
  }

  // initialize timers
  uint32_t ct = to_ms_since_boot(get_absolute_time());
  // first 8 timers are for each output and disabled by default
  for (uint8_t i = 0; i < 16; i++) {
    SimpleTimer_init(&pool_timer[i], 16, 1.0f, 0, timer_callback_outputs, i);
  }
  // setup a timer at 5 milliseconds to sample the knobs
  SimpleTimer_init(&pool_timer[8], 1000.0f / 11.0f * 30, 1.0f, 0,
                   timer_callback_sample_knob, 0);
  SimpleTimer_start(&pool_timer[8], ct);
  // setup a timer at 33 hz to update the ws2812
  SimpleTimer_init(&pool_timer[9], 1000.0f / 100.0f * 30, 1.0f, 0,
                   timer_callback_ws2812, 0);
  SimpleTimer_start(&pool_timer[9], ct);
  // setup a timer at 1 second to print memory usage
  SimpleTimer_init(&pool_timer[10], 1000.0f / 1000.0f * 30, 1.0f, 0,
                   timer_callback_print_memory_usage, 0);
  // SimpleTimer_start(&pool_timer[10], ct);
  // setup a timer at 4 ms intervals to update voltages
  SimpleTimer_init(&pool_timer[11], 1000.0f / 4.0f * 30, 1.0f, 0,
                   timer_callback_update_voltage, 0);
  SimpleTimer_start(&pool_timer[11], ct);

  uint32_t ct_last = ct;

  while (true) {
#ifdef INCLUDE_MIDI
    tud_task();
    midi_comm_task(midi_sysex_callback, midi_note_on, midi_note_off, NULL,
                   midi_start, midi_continue, midi_stop, midi_timing);
#endif

    while (uart_is_readable(UART_ID)) {
      uint8_t ch = (uint8_t)uart_getc(UART_ID);
      if (ch > 0) {
        printf("MIDI: %d\n", ch);
        midi_receive_byte(ch);
      }
    }

    // process timers
    for (uint8_t i = 0; i < 16; i++) {
      SimpleTimer_process(&pool_timer[i], ct);
    }

    // read buttons
    for (uint8_t i = 0; i < button_num; i++) {
      bool val = 1 - gpio_get(button_pins[i]);
      if (val != button_values[i]) {
        printf("Button %d: %d\n", i, val);
        button_values[i] = val;
        if (i < 8) {
          // process button press
          Out *out = &yocto.out[i];
          Config *config = &yocto.config[yocto.i][i];
          // check mode
          switch (config->mode) {
            case MODE_MANUAL:
              break;
            case MODE_ENVELOPE:
              // trigger the envelope
              ADSR_gate(&out->adsr, val, ct);
              break;
            default:
              break;
          }
        }
      }
    }

    // make sure the rest of the loop doesn't run faster than 500 hz
    ct = to_ms_since_boot(get_absolute_time());
    if (ct - ct_last < 2) {
      continue;
    }

    // yoctocore save (if debounced)
    start_time = time_us_64();
    if (Yoctocore_save(&yocto, ct)) {
      printf("saved data in %lld us\n", time_us_64() - start_time);
    }

    // process outputs
    for (uint8_t i = 0; i < 8; i++) {
      Out *out = &yocto.out[i];
      Config *config = &yocto.config[yocto.i][i];
      float knob_val = (float)KnobChange_get(&pool_knobs[i]);
      // check mode
      // make sure modes are up to date
      if (config->mode == MODE_CLOCK) {
        SimpleTimer_on(&pool_timer[i], ct);
      } else {
        SimpleTimer_stop(&pool_timer[i]);
      }
      // update slews
      Slew_set_duration(&out->portamento, roundf(config->portamento * 1000));
      Slew_set_duration(&out->slew, roundf(config->slew_time * 1000));

      switch (config->mode) {
        case MODE_MANUAL:
          // mode manual will set voltage based on knob turning and slew
          // check if knob was turned
          if (knob_val != -1) {
            // change the set voltage
            out->voltage_set = linlin(knob_val, 0.0f, 1023.0f,
                                      config->min_voltage, config->max_voltage);
          }
          // slew the voltage
          out->voltage_current = Slew_process(&out->slew, out->voltage_set, ct);
          // quantize the voltage
          out->voltage_current =
              scale_quantize_voltage(config->quantization, config->root_note,
                                     config->v_oct, out->voltage_current);
          // portamento voltage
          out->voltage_current =
              Slew_process(&out->portamento, out->voltage_current, ct);
          break;
        case MODE_LFO:
          // mode lfo will set the voltage based on lfo
          out->voltage_set = get_lfo_value(
              config->lfo_waveform, ct, config->lfo_period * 1000,
              config->min_voltage, config->max_voltage, 0, &out->noise);
          // quantize
          out->voltage_current =
              scale_quantize_voltage(config->quantization, config->root_note,
                                     config->v_oct, out->voltage_set);
          break;
        case MODE_PITCH:
          // mode pitch will set the voltage based on midi note
          // check if midi note was received
          // slew the voltage
          out->voltage_current = Slew_process(&out->slew, out->voltage_set, ct);
          // quantize the voltage
          out->voltage_current =
              scale_quantize_voltage(config->quantization, config->root_note,
                                     config->v_oct, out->voltage_current);
          // portamento voltage
          out->voltage_current =
              Slew_process(&out->portamento, out->voltage_current, ct);
          break;
        case MODE_CLOCK:
          break;
        case MODE_ENVELOPE:
          // mode envelope will trigger the envelope based on button press
          // knob changes will scale the attack/release
          if (knob_val != -1) {
            // scale the attack/release
            float attack, release;
            spiral_coordinate(knob_val, &attack, &release);
            attack = linlin(attack, 0.0f, 1.0f, 10.0f, 1000.0f);
            release = linlin(release, 0.0f, 1.0f, 10.0f, 5000.0f);
            config->attack = roundf(attack);
            config->release = roundf(release);
            printf("Attack: %f, Release: %f\n", attack, release);
          }
          out->adsr.attack = roundf(config->attack * 1000);
          out->adsr.decay = roundf(config->decay * 1000);
          out->adsr.sustain = config->sustain;
          out->adsr.release = roundf(config->release * 1000);
          out->voltage_set = linlin(ADSR_process(&out->adsr, ct), 0.0f, 1.0f,
                                    config->min_voltage, config->max_voltage);
          out->voltage_current = out->voltage_set;
          break;
        default:
          break;
      }
      // clamp voltages
      out->voltage_current = util_clamp(
          out->voltage_current, config->min_voltage, config->max_voltage);
    }
  }
}
