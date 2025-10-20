#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For sleep

#define __linux__ 1

#include "../../luavm.h"

int main() {
  printf("Testing Lua panic handling...\n\n");
  
  // Initialize Lua
  luaInit();
  
  // Test 1: Load code that will panic (calling non-existent function)
  const char *panic_code = 
      "function on_beat(on)\n"
      "  local x = nonexistent_function()\n"  // This will cause an error
      "  volts = x\n"
      "  trigger = on\n"
      "end\n";
  
  printf("Test 1: Loading code with non-existent function call...\n");
  int result = luaUpdateEnvironment(1, panic_code);
  if (result == 0) {
    printf("Code loaded successfully\n");
  } else {
    printf("Failed to load code\n");
  }
  
  // Test 2: Try to call the function and catch the error
  printf("\nTest 2: Calling on_beat with panicking code...\n");
  float volts;
  bool volts_new;
  bool trigger;
  int error = luaRunOnBeat(1, true, &volts, &volts_new, &trigger);
  if (error == 0) {
    printf("on_beat succeeded (unexpected)\n");
  } else if (error > 0) {
    printf("on_beat failed with error code %d (expected)\n", error);
  } else {
    printf("on_beat returned -1 (function not found)\n");
  }
  
  // Test 3: Load code that accesses undefined variable
  const char *panic_code2 = 
      "function on_knob(value)\n"
      "  volts = undefined_variable + 5\n"  // This will cause an error
      "  trigger = true\n"
      "end\n";
  
  printf("\nTest 3: Loading code with undefined variable...\n");
  result = luaUpdateEnvironment(2, panic_code2);
  if (result == 0) {
    printf("Code loaded successfully\n");
  }
  
  printf("\nTest 4: Calling on_knob with panicking code...\n");
  error = luaRunOnKnob(2, 0.5, &volts, &volts_new, &trigger);
  if (error == 0) {
    printf("on_knob succeeded (unexpected)\n");
  } else if (error > 0) {
    printf("on_knob failed with error code %d (expected)\n", error);
  } else {
    printf("on_knob returned -1 (function not found)\n");
  }
  
  // Test 5: Load valid code in a new environment
  const char *valid_code = 
      "function on_note_on(channel, note, velocity)\n"
      "  volts = note / 12.0\n"
      "  trigger = true\n"
      "end\n";
  
  printf("\nTest 5: Loading valid code into environment 4...\n");
  result = luaUpdateEnvironment(4, valid_code);
  if (result == 0) {
    printf("Code loaded successfully\n");
  }
  
  printf("\nTest 6: Calling on_note_on with valid code...\n");
  error = luaRunOnNoteOn(4, 1, 60, 127, &volts, &volts_new, &trigger);
  if (error == 0) {
    printf("on_note_on succeeded (expected), volts=%f, trigger=%d\n", volts, trigger);
  } else {
    printf("on_note_on failed with error code %d (unexpected)\n", error);
  }
  
  // Cleanup
  luaGarbageCollect();
  // Note: L is managed by luavm.h, so we don't close it here
  
  printf("\nAll tests completed!\n");
  
  return 0;
}
