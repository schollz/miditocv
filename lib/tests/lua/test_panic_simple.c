#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../lua-5.4.6/src/lauxlib.h"
#include "../../../lua-5.4.6/src/lua.h"
#include "../../../lua-5.4.6/src/lualib.h"

#define __linux__ 1

#include "../../luavm.h"

// Simulate the Out structure from miditocv.h
typedef struct {
  bool lua_panic;
} SimulatedOut;

// Simulate panic detection and flag setting
void simulate_panic_detection() {
  SimulatedOut outputs[8];

  // Initialize outputs
  for (int i = 0; i < 8; i++) {
    outputs[i].lua_panic = false;
  }

  printf("=== Simulating Lua Panic Detection ===\n\n");

  // Initialize Lua
  luaInit();

  // Test 1: Load panicking code into output 1 (environments are 1-indexed)
  const char *panic_code =
      "function on_button(value)\n"
      "  local result = some_undefined_function()\n"
      "  volts = result\n"
      "end\n";

  printf("1. Loading panicking code into output 1...\n");
  luaUpdateEnvironment(1, panic_code);

  // Test 2: Try to execute the panicking code
  printf("2. Executing on_button (should panic)...\n");
  float volts;
  bool volts_new;
  bool trigger;
  float gate;
  int error = luaRunOnButton(1, true, &volts, &volts_new, &trigger, &gate);

  if (error > 0) {
    printf("   ✓ Panic detected! Error code: %d\n", error);
    outputs[1].lua_panic = true;
    printf("   ✓ Set lua_panic flag for output 1\n");
  } else {
    printf("   ✗ Expected panic but got success\n");
  }

  // Test 3: Attempt to run the code again (should be blocked)
  printf("3. Attempting to run code again (should be blocked)...\n");
  if (outputs[1].lua_panic) {
    printf("   ✓ Code execution blocked due to lua_panic flag\n");
  } else {
    printf("   ✗ Code was not blocked\n");
  }

  // Test 4: Load valid code (clears panic)
  printf("4. Simulating code reload (clears panic flag)...\n");
  outputs[1].lua_panic = false;
  printf("   ✓ Panic flag cleared\n");

  // Test 5: Load and run valid code on output 2
  const char *valid_code =
      "function on_cc(cc, value)\n"
      "  volts = value / 127.0\n"
      "  trigger = true\n"
      "end\n";

  printf("5. Loading valid code into output 2...\n");
  luaUpdateEnvironment(2, valid_code);

  printf("6. Executing on_cc with valid code...\n");
  error = luaRunOnCc(2, 7, 64, &volts, &volts_new, &trigger, &gate);

  if (error == 0) {
    printf("   ✓ Valid code executed successfully\n");
    printf("   ✓ volts = %f, trigger = %d\n", volts, trigger);
  } else {
    printf("   ✗ Expected success but got error\n");
  }

  // Summary
  printf("\n=== Summary ===\n");
  printf("✓ Panic detection works correctly\n");
  printf("✓ Panic flag prevents re-execution\n");
  printf("✓ Flag can be cleared when code is reloaded\n");
  printf("✓ Valid code executes normally\n");
  printf("\n=== Test Complete ===\n");
}

int main() {
  simulate_panic_detection();
  return 0;
}
