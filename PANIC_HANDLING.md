# Lua Panic Handling Implementation

## Overview
This document describes the implementation of Lua panic handling for the miditocv project.

## Problem Statement
When an output is set to Code mode and Lua code is uploaded that causes a panic (runtime error), the system needs to:
1. Catch and flag the panic
2. Stop running the panicking code
3. Prevent the code from running again
4. Provide visual feedback by blinking RED/OFF on the output's LED

## Implementation

### 1. Data Structure Changes

**File: `lib/miditocv.h`**

Added `lua_panic` flag to the `Out` structure:
```c
typedef struct Out {
  // ... existing fields ...
  bool lua_panic;  // Flag to indicate Lua code has panicked
  // ... existing fields ...
} Out;
```

The flag is initialized to `false` in `Yoctocore_init()`.

### 2. Lua Callback Return Values

**File: `lib/luavm.h`**

Changed all Lua callback functions to return `int` instead of `bool`:
- `luaRunOnBeat()` 
- `luaRunOnKnob()`
- `luaRunOnButton()`
- `luaRunOnNoteOn()`
- `luaRunOnNoteOff()`
- `luaRunOnCc()`

**Return Values:**
- `0` - Success, function executed without errors
- `> 0` - Lua runtime error occurred (panic)
- `-1` - Function not found or environment not available

### 3. Panic Detection and Prevention

**File: `main.c`**

Updated all Lua callback invocations to:
1. Check if `lua_panic` flag is set before running code
2. Capture the return value from Lua callbacks
3. Set the `lua_panic` flag when an error is detected
4. Log the panic event

Example from `timer_callback_beat()`:
```c
if (config->mode == MODE_CODE) {
  // Don't run if Lua code has panicked
  if (out->lua_panic) {
    return;
  }
  float volts;
  bool volts_new;
  bool trigger;
  int result = luaRunOnBeat(user_data, on, &volts, &volts_new, &trigger);
  if (result == 0) {
    on_successful_lua_callback(user_data, volts, volts_new, trigger);
  } else if (result > 0) {
    // Lua panic occurred
    out->lua_panic = true;
    printf("[output%d] Lua panic detected, disabling code execution\n", user_data);
  }
}
```

This pattern is applied to:
- `timer_callback_beat()` - for on_beat callbacks
- `timer_callback_sample_knob()` - for on_knob callbacks  
- `midi_note_on()` - for on_note_on callbacks
- `midi_note_off()` - for on_note_off callbacks
- `midi_cc()` - for on_cc callbacks
- Button handling code - for on_button callbacks

### 4. Visual Feedback

**File: `main.c`**

Updated `timer_callback_ws2812()` to blink RED when `lua_panic` is set:
```c
if (out->lua_panic) {
  // Blink RED for panicked Lua code
  if (blink_on) {
    WS2812_fill(&ws2812, leds_second_8[i - 8] + 8, 255, 0, 0);
  } else {
    WS2812_fill(&ws2812, leds_second_8[i - 8] + 8, 0, 0, 0);
  }
}
```

The LED alternates between RED and OFF using the existing `blink_on` state.

### 5. Panic Flag Clearing

**File: `main.c`**

The `lua_panic` flag is cleared when:

1. **Mode changes to CODE mode**: New code is loaded and flag is cleared
2. **Mode changes away from CODE mode**: Flag is cleared for consistency
3. **New code is uploaded**: Flag is cleared when code_updated is detected

```c
if (config->mode != out->mode_last) {
  out->mode_last = config->mode;
  switch (config->mode) {
    case MODE_CODE:
      // load the new code environment and clear panic flag
      out->lua_panic = false;
      Yoctocore_load_code(&yocto, yocto.i, i);
      break;
    default:
      // Clear panic flag when switching to non-CODE mode
      out->lua_panic = false;
      break;
  }
} else {
  // special case - check if code was updated for any scene
  if (out->code_updated) {
    Config *uploaded_config = &yocto.config[out->code_updated_scene][i];
    if (uploaded_config->mode == MODE_CODE) {
      // load the new code from the correct scene and clear panic flag
      out->lua_panic = false;
      Yoctocore_load_code(&yocto, out->code_updated_scene, i);
    }
    out->code_updated = false;
  }
}
```

## Testing

### Test File: `lib/tests/lua/test_panic_simple.c`

Created a comprehensive test that validates:
1. **Panic Detection**: Lua errors are correctly detected and flagged
2. **Execution Prevention**: Code execution is blocked when panic flag is set
3. **Flag Clearing**: Panic flag can be cleared when new code is loaded
4. **Normal Operation**: Valid code executes normally

### Test Results
```
✓ Panic detection works correctly
✓ Panic flag prevents re-execution
✓ Flag can be cleared when code is reloaded
✓ Valid code executes normally
```

## Error Scenarios Caught

The implementation catches various Lua runtime errors:
- Calling undefined functions
- Accessing undefined variables
- Type errors (e.g., arithmetic on nil values)
- Table access errors
- Any other Lua runtime errors caught by `lua_pcall()`

## User Experience

When a Lua panic occurs:
1. The error is logged to the console with output number
2. The output's LED begins blinking RED/OFF immediately
3. The panicking code stops executing completely
4. The user can fix the code and upload new code
5. When new code is uploaded, the panic flag clears and the LED returns to normal

## Backward Compatibility

The implementation maintains backward compatibility:
- No changes to the Lua API exposed to users
- No changes to the MIDI message format
- No changes to the storage format
- Existing non-panicking code continues to work identically

## Future Improvements

Potential enhancements:
- Store panic error message for display
- Add a way to clear panic flag via button press
- Implement panic statistics/logging
- Add panic recovery with automatic retry after timeout
