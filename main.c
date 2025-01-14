#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "pico/binary_info.h"
#include "pico/bootrom.h"
#include "pico/stdlib.h"
#include "pico/types.h"

int main() {
  // Reinit uart now that clk_peri has changed
  stdio_init_all();

  sleep_ms(10);
  // setup GPIO25
  gpio_init(25);
  gpio_set_dir(25, GPIO_OUT);
  gpio_init(23);
  gpio_set_dir(23, GPIO_IN);
  uint32_t count = 0;
  bool previous_state = false;
  while (true) {
    // get the state of GPIO23
    bool state = gpio_get(23);
    if (state != previous_state) {
      if (state) {
        gpio_put(25, 1);
        count++;
        printf("count: %d\n", count);
        if (count == 10) {
          // create a memory fault
          int *p = 0;
          *p = 0;
        }
      } else {
        gpio_put(25, 0);
      }
      previous_state = state;
    }
    sleep_ms(1);
  }
}
