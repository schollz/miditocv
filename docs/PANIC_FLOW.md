# Lua Panic Handling Flow

## Normal Operation (No Panic)

```
┌─────────────────────────────────────────────────────────┐
│ User uploads Lua code to output                         │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Code is stored to SD card                               │
│ code_updated flag is set                                │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Main loop detects code_updated                          │
│ Clears lua_panic flag                                   │
│ Loads code via luaUpdateEnvironment()                   │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Event occurs (button press, MIDI message, etc.)         │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Check if lua_panic flag is set                          │
│ ✓ Flag is false, continue                               │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Call Lua callback (e.g., luaRunOnButton)                │
│ Returns 0 (success)                                      │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Execute on_successful_lua_callback()                    │
│ Set voltage, trigger outputs                            │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ LED shows normal color for mode                         │
└─────────────────────────────────────────────────────────┘
```

## Panic Detection and Handling

```
┌─────────────────────────────────────────────────────────┐
│ User uploads buggy Lua code to output                   │
│ (e.g., calls undefined function)                        │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Code is stored and loaded                               │
│ lua_panic flag is cleared                               │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Event occurs (e.g., button press)                       │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Check if lua_panic flag is set                          │
│ ✓ Flag is false, continue                               │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Call Lua callback (e.g., luaRunOnButton)                │
│   ├─> lua_pcall() executes the on_button function       │
│   ├─> Lua error occurs!                                 │
│   │    "attempt to call a nil value"                    │
│   └─> lua_pcall() returns LUA_ERRRUN (2)                │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Callback returns error code 2                           │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ main.c detects error (result > 0)                       │
│ ✓ Set lua_panic flag = true                             │
│ ✓ Log: "[output%d] Lua panic detected..."              │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ LED begins blinking RED/OFF                             │
│ (in timer_callback_ws2812)                              │
└─────────────────────────────────────────────────────────┘
```

## Subsequent Executions After Panic

```
┌─────────────────────────────────────────────────────────┐
│ Another event occurs                                    │
│ (button press, timer tick, MIDI, etc.)                  │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Check if lua_panic flag is set                          │
│ ✗ Flag is TRUE!                                         │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Return immediately without calling Lua                  │
│ (Code execution blocked)                                │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ LED continues blinking RED/OFF                          │
│ (Visual indication of panic state)                      │
└─────────────────────────────────────────────────────────┘
```

## Recovery Process

```
┌─────────────────────────────────────────────────────────┐
│ User fixes the code and uploads new version             │
│ OR                                                       │
│ User changes output mode away from CODE                 │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ Main loop detects code_updated or mode change           │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ ✓ Clear lua_panic flag = false                          │
│ ✓ Load new code (if MODE_CODE)                          │
└────────────────┬────────────────────────────────────────┘
                 │
                 ▼
┌─────────────────────────────────────────────────────────┐
│ System returns to normal operation                      │
│ LED shows normal mode color                             │
└─────────────────────────────────────────────────────────┘
```

## State Diagram

```
                  ┌──────────────┐
                  │   NORMAL     │
                  │  (no panic)  │
                  └──────┬───────┘
                         │
                         │ Lua error occurs
                         │
                         ▼
                  ┌──────────────┐
                  │   PANICKED   │◄───┐
                  │ (LED blinks) │    │
                  └──────┬───────┘    │
                         │            │
                         │            │ More events
                         │            │ (blocked)
                         │            │
                         │            └────────┘
                         │
                         │ New code uploaded
                         │ OR mode changed
                         │
                         ▼
                  ┌──────────────┐
                  │   CLEARED    │
                  │  (recovered) │
                  └──────┬───────┘
                         │
                         │
                         ▼
                  ┌──────────────┐
                  │   NORMAL     │
                  │  (no panic)  │
                  └──────────────┘
```

## LED States

| State | LED Behavior | Description |
|-------|--------------|-------------|
| Normal CODE mode | Violet (mode color) | Code executes normally |
| Panicked CODE mode | RED/OFF blink | Lua error detected, execution blocked |
| After recovery | Violet (mode color) | Panic cleared, ready for execution |
| Non-CODE mode | Mode-specific color | Panic flag cleared when leaving CODE mode |

## Error Codes

| Return Value | Meaning | Action |
|--------------|---------|--------|
| 0 | Success | Execute callback results |
| -1 | Function not found | Ignore (no function defined) |
| 2 | LUA_ERRRUN | Set panic flag, block execution |
| 3 | LUA_ERRMEM | Set panic flag, block execution |
| 4 | LUA_ERRERR | Set panic flag, block execution |

## Key Points

1. **Immediate Response**: Panic is detected and handled on the first error
2. **Complete Block**: All Lua callbacks are blocked once panic flag is set
3. **Visual Feedback**: RED blinking continues until recovery
4. **Easy Recovery**: Simply upload new code or change mode
5. **No Manual Reset**: System automatically clears flag on code update
6. **Safe Operation**: Device remains functional, only affected output is blocked
