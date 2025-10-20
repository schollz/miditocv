# Lua Tests

This directory contains tests for the Lua VM integration in miditocv.

## Building and Running Tests

### Prerequisites
```bash
sudo apt-get install lua5.3 liblua5.3-dev
npm install -g luamin
```

### Run Tests

**Main test (original):**
```bash
make run
```

**Panic handling test (simplified):**
```bash
gcc -o test_panic_simple test_panic_simple.c -llua5.3 -lm
./test_panic_simple
```

## Tests

### main.c
The original test suite that validates basic Lua functionality including:
- Environment setup
- Code loading
- Callback execution (on_beat)
- Variable access (bpm)
- Sequins library

### test_panic_simple.c
Comprehensive test for Lua panic handling that validates:
- ✓ Panic detection works correctly
- ✓ Panic flag prevents re-execution  
- ✓ Flag can be cleared when code is reloaded
- ✓ Valid code executes normally

This test simulates the complete panic handling workflow:
1. Load panicking code
2. Execute it (triggers panic)
3. Verify panic flag is set
4. Verify execution is blocked
5. Clear the flag (simulating code reload)
6. Load and execute valid code successfully

### test_panic.c
Extended test that explores various panic scenarios (may crash due to pre-existing Lua environment management issues).

## Expected Output

When running test_panic_simple, you should see:
```
=== Simulating Lua Panic Detection ===

1. Loading panicking code into output 0...
2. Executing on_button (should panic)...
   ✓ Panic detected! Error code: 2
   ✓ Set lua_panic flag for output 0
3. Attempting to run code again (should be blocked)...
   ✓ Code execution blocked due to lua_panic flag
4. Simulating code reload (clears panic flag)...
   ✓ Panic flag cleared
5. Loading valid code into output 1...
6. Executing on_cc with valid code...
   ✓ Valid code executed successfully
   ✓ volts = 0.503937, trigger = 1

=== Summary ===
✓ Panic detection works correctly
✓ Panic flag prevents re-execution
✓ Flag can be cleared when code is reloaded
✓ Valid code executes normally

=== Test Complete ===
```

## Integration

These tests use the same Lua VM code (`../../luavm.h`) that is used in the main miditocv firmware, ensuring that the panic handling behavior tested here matches the actual device behavior.
