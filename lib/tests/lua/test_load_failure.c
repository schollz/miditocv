#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>

#define __linux__ 1

#include "../../luavm.h"

// Test that code loading failures are properly detected
void test_load_failure_detection() {
  printf("=== Testing Code Load Failure Detection ===\n\n");
  
  // Initialize Lua
  printf("1. Initializing Lua...\n");
  assert(luaInit() == 0);
  printf("   ✓ Lua initialized\n");
  
  // Test 1: Load valid code - should succeed
  printf("\n2. Testing valid code load...\n");
  const char *valid_code = 
      "bpm = 120\n"
      "function on_beat(on)\n"
      "  volts = 5.0\n"
      "  trigger = on\n"
      "  return 'ok'\n"
      "end\n";
  
  int result = luaUpdateEnvironment(0, valid_code);
  printf("   Result: %d\n", result);
  assert(result == 0);
  printf("   ✓ Valid code loaded successfully\n");
  
  // Test 2: Load code that calls undefined function at top level - should fail
  printf("\n3. Testing code with undefined function call at top level...\n");
  const char *invalid_code = 
      "bpm = 120\n"
      "local x = undefined_function()\n"  // This will fail at load time
      "function on_beat(on)\n"
      "  volts = 5.0\n"
      "end\n";
  
  result = luaUpdateEnvironment(1, invalid_code);
  printf("   Result: %d\n", result);
  assert(result == 1);
  printf("   ✓ Invalid code load detected correctly\n");
  
  // Test 3: Load code with syntax error - should fail
  printf("\n4. Testing code with syntax error...\n");
  const char *syntax_error_code = 
      "bpm = 120\n"
      "function on_beat(on\n"  // Missing closing paren
      "  volts = 5.0\n"
      "end\n";
  
  result = luaUpdateEnvironment(2, syntax_error_code);
  printf("   Result: %d\n", result);
  assert(result == 1);
  printf("   ✓ Syntax error detected correctly\n");
  
  // Test 4: The problematic code from the issue
  printf("\n5. Testing the issue code (from GitHub issue)...\n");
  const char *issue_code = 
      "note_vals = S { 'c4', 'e4', 'g4', S {'c5', 'e5'}, 'g5' }\n"
      "riddim = er(5, 8)\n"
      "bpm = 120\n"
      "function on_beat(on)\n"
      "  if on then\n"
      "    local note = note_vals()\n"
      "    trigger = riddim()\n"
      "    volts = to_cv(note)\n"
      "    local fn = new_fun()\n"  // undefined function
      "    volts = new_fun()\n"     // undefined function
      "    return note\n"
      "  end\n"
      "  return 'off'\n"
      "end\n";
  
  // This code should LOAD successfully (no errors at top level)
  // but will fail at RUNTIME when on_beat is called
  result = luaUpdateEnvironment(3, issue_code);
  printf("   Load result: %d\n", result);
  assert(result == 0);
  printf("   ✓ Issue code loaded (errors are at runtime, not load time)\n");
  
  // Now try to execute it - this should fail
  printf("   Executing on_beat...\n");
  float volts;
  bool volts_new;
  bool trigger;
  int exec_result = luaRunOnBeat(3, true, &volts, &volts_new, &trigger);
  printf("   Execution result: %d\n", exec_result);
  assert(exec_result > 0);  // Should return error code
  printf("   ✓ Runtime error detected correctly\n");
  
  // Test 5: Code that references undefined variable at top level
  printf("\n6. Testing code with undefined variable at top level...\n");
  const char *undefined_var_code = 
      "bpm = some_undefined_var\n"
      "function on_beat(on)\n"
      "  volts = 5.0\n"
      "end\n";
  
  result = luaUpdateEnvironment(4, undefined_var_code);
  printf("   Result: %d\n", result);
  // This should succeed because referencing undefined variable is not an error until it's accessed
  assert(result == 0);
  printf("   ✓ Code with undefined variable reference loaded (error would be at access time)\n");
  
  printf("\n=== Summary ===\n");
  printf("✓ Valid code loads successfully (returns 0)\n");
  printf("✓ Code with top-level errors fails to load (returns 1)\n");
  printf("✓ Code with syntax errors fails to load (returns 1)\n");
  printf("✓ Code with runtime errors loads but fails at execution\n");
  printf("✓ Panic detection works for both load-time and runtime errors\n");
  printf("\n=== Test Complete ===\n");
}

int main() {
  test_load_failure_detection();
  return 0;
}
