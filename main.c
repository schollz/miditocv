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
#include "hardware/pio.h"
#include "hardware/pll.h"
#include "hardware/rtc.h"
#include "hardware/structs/clocks.h"
#include "hardware/sync.h"
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
#include "lib/mcp3208.h"
#include "lib/memusage.h"
#include "lib/pcg_basic.h"
#include "lib/random.h"
#include "lib/scales.h"
#include "lib/sdcard.h"
#include "lib/simpletimer.h"
#include "lib/spiral.h"
#include "lib/yoctocore.h"

Yoctocore yocto;
DAC dac;
WS2812 ws2812;
SimpleTimer pool_timer[16];
KnobChange pool_knobs[8];
MCP3208 mcp3208;
const uint8_t button_num = 9;
const uint8_t button_pins[9] = {8, 9, 20, 21, 22, 26, 27, 28, 0};
uint8_t button_values[9] = {0, 0, 0, 0, 0, 0};

#ifdef INCLUDE_MIDI
#include "lib/midi_comm.h"
#include "lib/midicallback.h"
#endif

void timer_callback_outputs(bool on, int user_data) {
  // printf("[timer_callback_outputs]: %d %d\n", on, user_data);
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

void timer_callback_ws2812(bool on, int user_data) {
  for (uint8_t i = 0; i < 8; i++) {
    uint8_t val =
        roundf(linlin(yocto.out[i].voltage_current, -5.0, 10.0, 0.0, 255.0));
    WS2812_fill(&ws2812, i, val, 0, 255 - val);
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
  tusb_init();
#endif

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
  WS2812_init(&ws2812, WS2812_PIN, pio0, WS2812_SM, 8);
  WS2812_set_brightness(&ws2812, 70);
  for (uint8_t i = 0; i < 8; i++) {
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
  SimpleTimer_init(&pool_timer[9], 60 * 15, 1.0f, 0, timer_callback_ws2812, 0);
  SimpleTimer_start(&pool_timer[9], ct);
  // setup a timer at 1 second to print memory usage
  SimpleTimer_init(&pool_timer[10], 1000.0f / 1000.0f * 30, 1.0f, 0,
                   timer_callback_print_memory_usage, 0);
  // SimpleTimer_start(&pool_timer[10], ct);
  // setup a timer at 4 ms intervals to update voltages
  SimpleTimer_init(&pool_timer[11], 1000.0f / 4.0f * 30, 1.0f, 0,
                   timer_callback_update_voltage, 0);
  SimpleTimer_start(&pool_timer[11], ct);

  while (true) {
#ifdef INCLUDE_MIDI
    tud_task();
    midi_comm_task(midi_sysex_callback, midi_note_on, midi_note_off, midi_start,
                   midi_continue, midi_stop, midi_timing);
#endif

    // get time
    ct = to_ms_since_boot(get_absolute_time());

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
        case MODE_ENVELOPE:
          // mode envelope will trigger the envelope based on button press
          // knob changes will scale the attack/release
          if (knob_val != -1) {
            // scale the attack/release
            float attack, release;
            spiral_coordinate(knob_val, &attack, &release);
            attack = linlin(attack, 0.0f, 1.0f, 10.0f, 1000.0f);
            release = linlin(release, 0.0f, 1.0f, 10.0f, 5000.0f);
            out->adsr.attack = roundf(attack);
            out->adsr.release = roundf(release);
            printf("Attack: %f, Release: %f\n", attack, release);
          }

          out->voltage_set = linlin(ADSR_process(&out->adsr, ct), 0.0f, 1.0f,
                                    config->min_voltage, config->max_voltage);
          out->voltage_current = out->voltage_set;
        default:
          break;
      }
    }
  }
}
