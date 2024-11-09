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
#include "lib/flashmem.h"
#include "lib/knob_change.h"
#include "lib/mcp3208.h"
#include "lib/memusage.h"
#include "lib/pcg_basic.h"
#include "lib/random.h"
#include "lib/sdcard.h"
// globals
float g_bpm = 120.0;
DAC *dac;
WS2812 *ws2812;
ADSR *adsr[8];

#ifdef INCLUDE_MIDI
#include "lib/midi_comm.h"
#include "lib/midicallback.h"
#endif

#define FLASH_TARGET_OFFSET \
  (1792 * 1024)  //++ Starting Flash Storage location after 1.8MB ( of the 2MB )
const uint8_t *flash_target_contents =
    (const uint8_t *)(XIP_BASE +
                      FLASH_TARGET_OFFSET);  //++ Pointer pointing at the Flash
                                             // Address Location

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

  // Implicitly called by disk_initialize,
  // but called here to set up the GPIOs
  // before enabling the card detect interrupt:
  sd_init_driver();

  // initialize random library
  random_initialize();

  // // initialize MCP3208
  // MCP3208 *mcp3208 =
  //     MCP3208_malloc(spi0, PIN_SPI_CSN, PIN_SPI_CLK, PIN_SPI_RX, PIN_SPI_TX);

  // // initialize WS2812
  // ws2812 = WS2812_new(WS2812_PIN, pio0, WS2812_SM, WS2812_NUM_LEDS);
  // WS2812_set_brightness(ws2812, 50);
  // for (int i = 0; i < WS2812_NUM_LEDS; i++) {
  //   WS2812_fill(ws2812, i, 150, 0, 255);
  // }
  // WS2812_show(ws2812);

  // // initialize SD card
  // printf("[main]: initializing sd card\n");
  // for (uint8_t i = SDCARD_CMD_GPIO - 1; i < SDCARD_D0_GPIO + 5; i++) {
  //   gpio_pull_up(i);
  // }
  // if (!run_mount()) {
  //   sleep_ms(1000);
  //   printf("[main]: failed to mount sd card\n");
  // } else {
  //   big_file_test("test.bin", 2, 0);  // perform read/write test
  // }

  uint32_t ct = to_ms_since_boot(get_absolute_time());
  uint32_t ct_last_print = ct;
  uint32_t ct_next_bpm = ct + (60.0 / g_bpm * 1000);
  while (true) {
#ifdef INCLUDE_MIDI
    tud_task();
    midi_comm_task(midi_sysex_callback, midi_note_on, midi_note_off, midi_start,
                   midi_continue, midi_stop, midi_timing);
#endif

    ct = to_ms_since_boot(get_absolute_time());
    if (ct - ct_last_print > 1200) {
      ct_last_print = ct;
      uint32_t ct2 = to_ms_since_boot(get_absolute_time());
      print_memory_usage();
      flash_mem_test();
    }

    // // read knobs
    // for (uint8_t i = 0; i < 8; i++) {
    //   uint16_t val = MCP3208_read(mcp3208, i, false);
    //   printf("Knob %d: %d\n", i, val);
    // }

    // if (ct > ct_next_bpm) {
    //   ct_next_bpm = ct + (60.0 / g_bpm * 1000);
    //   printf("BPM: %f\n", g_bpm);
    // }
    sleep_us(1);
  }
}
