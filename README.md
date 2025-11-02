<p align="center"> <a href="https://www.youtube.com/watch?v=9bbGdvTweDQ"> <img
width="600" alt="miditocv"
src="https://github.com/user-attachments/assets/8677d7c9-285d-48c7-8f19-c4101e0e66c3"
/> </a> <br> <a
href="https://github.com/schollz/miditocv/releases/latest"> <img
src="https://img.shields.io/github/v/release/schollz/miditocv"
alt="Version"> </a> <a
href="https://github.com/schollz/miditocv/actions/workflows/build.yml">
<img
src="https://github.com/schollz/miditocv/actions/workflows/build.yml/badge.svg"
alt="Build Status"> </a> <a href="https://github.com/sponsors/schollz"> <img
alt="GitHub Sponsors" src="https://img.shields.io/github/sponsors/schollz">
</a> </p>

there are eight scenes. each scene is selected by TBD.

each scene has eight outputs. each output can be configured to a different mode. modes are selected holding `shift` and pressing one of the eight buttons.

The **bold** things are implemented.

| Category         | Button         | Knob       | Shift + Button  | Shift + Knob      | Button + Knob |
| ---------------- | -------------- | ---------- | --------------- | ----------------- | ------------- |
| Note             |                |            | **tuning note** | **change tuning** | **voltage**   |
| Envelope         | **trigger**    | **attack** |                 | **sustain**       | **release**   |
| Gate             | **trigger**    |            |                 | **probability**   |               |
| CC               | **listen**     |            |                 |                   |               |
| Program change   | **listen**     |            |                 |                   |               |
| Key Pressure     | **listen**     |            |                 |                   |               |
| Channel Pressure | **listen**     |            |                 |                   |               |
| Pitch bend       | **listen**     |            |                 |                   |               |
| Clock            | **start/stop** |            | **tap tempo**   | **division**      | **tempo**     |
| LFO              |                | **period** | **pause**       | **max voltage**   | **shape**     |
| Code             | on_button(on)  | on_knob(x) | on_button(on)   | on_knob(x)        |               |

## Output Linking

Envelope and Gate outputs can be linked to other outputs (like Clock or Code outputs) to create synchronized timing relationships. This is configured in the editor by setting the "linked to" parameter.

### Linking to Clock Outputs

When an Envelope or Gate output is linked to a Clock output:
- The linked output triggers whenever the Clock output goes high
- The linked output stays high for the duration of the Clock's high phase
- The duration is automatically calculated based on the Clock's BPM and division settings
- Clock division affects timing: higher divisions create shorter gate durations

**Example use case**: Link an Envelope output to a Clock output set to quarter notes (x1 division) at 120 BPM. The Envelope will trigger every quarter note and stay high for half the quarter note duration, creating a rhythmic gate pattern synchronized to the clock.

### Linking to Code Outputs

Code outputs can also trigger linked Envelope and Gate outputs. The `gate` parameter in Lua code controls how long the linked output stays high:
- `gate = 0.5` means the gate stays high for half a beat
- `gate = 1.0` means the gate stays high for a full beat
- The beat duration is determined by the Code output's BPM setting

This allows for complex, programmable triggering patterns while maintaining proper ADSR envelope behavior.

## Code

The miditocv is programmable with the [Lua language](https://www.lua.org/manual/5.4/manual.html), a powerful, efficient, lightweight, embeddable scripting language. Here is the implementation status:

- [x] [`volts`, `trigger`, and `gate`](#volts-trigger-and-gate)
- [x] [`out[i].volts`](#outivolts)
- [x] [`shift` and `button[i]`](#shift-and-buttoni)
- [x] [`on_beat(on)`](#on_beaton)
- [x] [`on_knob(value)`](#on_knobvalue)
- [x] [`on_button(value)`](#on_buttonvalue)
- [x] [`on_note_on(channel,note,velocity)`](#on_note_onchannelnotevelocity)
- [x] [`on_note_off(channel,note)`](#on_note_offchannelnote)
- [x] [`on_cc(channel,cc,value)`](#on_ccchannelccvalue)
- [x] [`on_key_pressure(channel,key,pressure)`](#on_key_pressurechannelkeypressure)
- [x] [`on_channel_pressure(channel,pressure)`](#on_channel_pressurechannelpressure)
- [x] [`on_pitch_bend(channel,pitch)`](#on_pitch_bendchannelpitch)
- [x] [`er(k,n,w)`](#erknw)
- [x] [`to_cv(value)`](#to_cvvalue)
- [x] [`S` - A Minimal Sequencing Library](#s---a-minimal-sequencing-library)

The miditocv has [an online editor](https://my.miditocv.com) that you can use to test out your programs.

miditocv programs are based around a set of [callback functions](<https://en.wikipedia.org/wiki/Callback_(computer_programming)>) that are run on specific routines.

### `volts`, `trigger`, and `gate`

For any function you can set the voltage, raise a trigger, and control the gate duration. The voltage is a number between -5 and 10, the trigger can be a boolean, number, or nil, and the gate is a number between 0 and 1 that controls how long the trigger stays high. To set the voltage you set the global variable `volts`:

```lua
volts = 5
```

To set the trigger you set the global variable `trigger`:

```lua
trigger = true    -- boolean true triggers
trigger = 1       -- any non-zero number triggers
trigger = 0       -- zero does not trigger (C-like behavior)
trigger = nil     -- nil does not trigger
```

To set the gate duration you set the global variable `gate`:

```lua
gate = 0      -- trigger goes high and immediately low (default)
gate = 0.5    -- trigger stays high for half a beat duration
gate = 1.0    -- trigger stays high for the full beat duration
```

The gate parameter is useful for controlling ADSR envelopes, particularly for setting the release stage. The trigger only triggers on low to high transitions. The trigger and gate will be reset after they are used. You can also use `out[i].trigger` and `out[i].gate` to set these for specific outputs.

You can also return values from the functions, which is useful in debugging as it will printed in the online code editor.

### `out[i].volts`

The `out[i].volts` is a table that can be used to set the voltage for a specific output. The `i` is the output number (1-8). The voltage is a number between -5 and 10. For example, if you are writing code for output 1, but want to change the voltage on output 8 you can write:

```lua
out[8].volts = 5
```

which will set the voltage on output 8 to 5V.

The code from any output can change the voltage for any other output. When this voltage _changes_ it will _override_ any other voltage on any other output.

### `shift` and `button[i]`

The state of buttons is accessible at any time in lua code using the `shift` and `button[i]` global state variables. They are boolean values.

```lua
if shift then print("shift on") else print("shift off") end
if button[1] then print("button 1 on") else print("button 1 off") end
```

### `on_beat(on)`

This function is called every time a beat is detected. The `on` is a boolean for whether it is on the top of the beat or the bottom.

```lua
bpm = 120 -- define bpm
local voltage_increase = 0
function on_beat(on)
    voltage_increase = voltage_increase + 0.1
    if voltage_increase > 10 then
        voltage_increase = 0
    end
    volts = voltage_increase
    trigger = on
end
```

This function is special, in that the Code output _can be linked to a Clock output_. If linked to a Clock output and the `bpm` variable is not defined, the Code output will use the Clock output's BPM. If the `bpm` variable is defined, it takes precedence. If neither is set, it will run at the global tempo. Note that linking only synchronizes the BPM; the Code output is not triggered by the Clock output.

### `on_knob(value)`

This function exposes the knob events. The `value` is the knob value (0.0-1.0), the `shift` is a boolean of whether the shift button is pressed.

```lua
function on_knob(value)
    if shift then
        volts = value
    else
        volts = value * 5.0
    end
end
```

### `on_button(value)`

This function exposes the button event. The `value` is `true` if the button is pressed, else `false`, and the `shift` is a boolean of whether the shift button is pressed.

```lua
function on_button(value)
    if shift then
        trigger = value
    else
        volts = value and 5 or 0
    end
end
```

### `on_note_on(channel,note,velocity)`, `on_note_off(channel,note)`

These functions expose the MIDI note on and note off events. The `channel` is the MIDI channel, the `note` is the MIDI note, and the `velocity` is the MIDI velocity.

```lua
function on_note_on(channel,note,velocity)
    volts = (note-60.0)/12.0
    trigger = true
end

function on_note_off(channel,note)
    trigger = false
end
```

### `on_cc(channel,cc,value)`

This function exposes the MIDI CC events. The `channel` is the MIDI channel, the `cc` is the MIDI CC number, and the `value` is the MIDI value.

```lua
function on_cc(channel,cc,value)
    volts = value/127.0
end
```

### `on_key_pressure(channel,key,pressure)`

This function exposes the MIDI key pressure events. The `channel` is the MIDI channel, the `key` is the MIDI key, and the `pressure` is the MIDI pressure.

```lua
function on_key_pressure(channel,key,pressure)
    volts = pressure/127.0
end
```

### `on_channel_pressure(channel,pressure)`

This function exposes the MIDI channel pressure events. The `channel` is the MIDI channel, and the `pressure` is the MIDI pressure.

```lua
function on_channel_pressure(channel,pressure)
    if (channel == 1) then
        volts = pressure/127.0
    end
end
```

### `on_pitch_bend(channel,pitch)`

This function exposes the MIDI pitch bend events. The `channel` is the MIDI channel, and the `pitch` is the MIDI pitch as a 14-bit number.

```lua
function on_pitch_bend(channel,pitch)
    volts = pitch/16383.0
end
```

## Code Helper Functions

### `er(k,n,w)`

This function generates a sequins sequence for euclidean rhythms. The `k` is the number of hits, the `n` is the number of steps, and the `w` is the rotation of the sequence.

```lua
riddim = er(3,8,0) -- generates a euclidean rhythm with 3 hits in 8 steps
function on_beat(on)
    if on then
        trigger = riddim()
    end
end
```

### `to_cv(value)`

This function is called every time a value is sent to the CV output. The `value` is the voltage that is being sent to the CV output. The value can be a MIDI note (values between 10 and 127) or it can be a voltage (values between -5 and 10) or it can be a note name (e.g. "c4").

```lua
volts = to_cv(60) -- sets the voltage to 0V
volts = to_cv("a5") -- sets the voltage to 1.75V
volts = to_cv(0) -- sets the voltage to 0V
```

### `linlin(x, in_min, in_max, out_min, out_max)`

This function performs linear interpolation from one range to another. It takes an input value `x` and maps it from the range `[in_min, in_max]` to the range `[out_min, out_max]`.

```lua
linlin_value = linlin(5, 0, 10, -5, 5) -- maps 5 from range [0, 10] to range [-5, 5]
```

### S - A Minimal Sequencing Library

`S` is a library designed to build sequencers and arpeggiators with minimal scaffolding using Lua tables. Originally designed by [Trent Gill](https://monome.org/docs/norns/reference/lib/sequins), it provides a simple, extensible interface for creating complex patterns with ease.

This document introduces the basics of `S`. For advanced techniques, see the extended reference.

#### Syntax and Description

| **Syntax**                   | **Description**                                                                     |
| ---------------------------- | ----------------------------------------------------------------------------------- |
| `my_seq = S{a, b, c, ...}`   | Create a sequence of values (any data type, including nested sequences).            |
| `my_seq()`                   | Call the sequence, advancing by the step size (default is 1) and returning a value. |
| `my_seq:step(x)`             | Change the step size to `x`.                                                        |
| `my_seq:select(n)`           | Select index `n` for the next call to `my_seq()`.                                   |
| `my_seq[x] = y`              | Update the value at index `x` to `y` (does not change the length).                  |
| `my_seq:settable(new_table)` | Replace the sequence's table with `new_table` (changes length and resets index).    |

---

#### Flow Modifiers

Flow modifiers work exclusively on nested sequences, offering additional control.

| **Syntax**        | **Description**                                                           |
| ----------------- | ------------------------------------------------------------------------- |
| `my_seq:every(n)` | Produce a value every `n`th call.                                         |
| `my_seq:times(n)` | Produce values only the first `n` times it's called.                      |
| `my_seq:count(n)` | Produce `n` values from the inner sequence before returning to the outer. |
| `my_seq:all()`    | Iterate through all inner sequence values before returning to the outer.  |
| `my_seq:reset()`  | Reset all flow modifiers and indices.                                     |

---

#### Examples

```lua
note_vals = S{'c4', 'd4', 'e4', S{'g4', 'a4', 'b4'}} -- plays c4, d4, e4, then g4, a4, b4
note_vals = S{'c4', 'd4', 'e4', S{'g4', 'a4', 'b4'}:every(3)}:step(3) -- Advance by 3, play inner sequence every 3rd iteration
note_vals = S{'c4', 'd4', 'e4', S{'g4', 'a4', 'b4'}:count(10)} -- Inner sequence iterates 10 times
note_vals = S{'c4', 'd4', 'e4', S{'f4', 'g4', 'a4'}:times(6)} -- Inner sequence iterates as normal but stops after 6 iterations
note_vals = S{'c4', 'd4', 'e4', S{'f4', 'g4', 'a4'}:all()} -- Inner sequence plays all values before releasing focus
```

## Calibration

_Note:_ This works on windows only.

```bash
cd dev
uv venv .venv --python=python3.12
.\.venv\Scripts\activate
uv pip install -r requirements.txt
.\.venv\Scripts\python.exe .\calibrate.py <ID>
```

The miditocv core is calibrated using an NI USB-6009 device to read in the voltages while manipulating the raw values to obtain a calibration curve for adjusting the final voltage.

To get started, you will need nidaqmx + drivers:

```
sudo -H python3 -m pip install --break-system-packages nidaqmx
sudo -H python3 -m nidaqmx installdriver # install everything
```

Restart the computer and make sure to DISABLE "Secure Boot".

To see if it installed correctly:

```
> nidaqmxconfig
```

For errors, see [this](https://knowledge.ni.com/KnowledgeArticleDetails?id=kA03q000000wwZyCAI&l=en-US).

## Building Lua for the browser

```
https://github.com/Doridian/LuaJS
cd LuaJS && git checkout 8cd117fdbec00a3b6a8a403757f62b700e41b445
git submodule update --init --recursive
npm install
npm run clean
npm run build
```

The `dist` folder can now be used.

## Debugger

This guide shows how to debug the miditocv firmware using GDB and OpenOCD with a CMSIS-DAP compatible debugger.

### Prerequisites

Install the required debugging tools:

```bash
sudo apt install gdb-multiarch openocd minicom
```

### Step-by-Step Debugging

#### 1. Build the firmware in debug mode

```bash
make miditocv-debug
```

This creates a debug build with symbols in the `build/` directory.

#### 2. Upload the debug firmware to the device

```bash
cd build && make && sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program miditocv.elf verify reset exit"
```

This uploads `miditocv.elf` to the RP2040 and verifies the upload.

#### 3. Start the OpenOCD debug server

In a **separate terminal**, start the OpenOCD server:

```bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
```

Leave this running. OpenOCD will listen on port 3333 for GDB connections.

#### 4. Connect GDB to the debug server

In another terminal, start GDB and connect to OpenOCD:

```bash
gdb-multiarch build/miditocv.elf -ex "target remote localhost:3333" -ex "monitor reset init" -ex "continue"
```

This connects to the OpenOCD server, resets the device, and starts execution.

### Common GDB Commands

Once connected, you can use these GDB commands:

```
(gdb) break main              # Set a breakpoint at main()
(gdb) break filename.c:123    # Set a breakpoint at line 123
(gdb) continue                # Continue execution
(gdb) step                    # Step into functions
(gdb) next                    # Step over functions
(gdb) backtrace               # Show the call stack
(gdb) print variable_name     # Print variable value
(gdb) info locals             # Show local variables
(gdb) quit                    # Exit GDB
```

### Monitoring Serial Output

To view serial output from the device during debugging, open another terminal and run:

```bash
minicom -b 115200 -o -D /dev/ttyACM1
```

**Note:** The device may appear on `/dev/ttyACM0` instead, depending on your system. Use `ls /dev/ttyACM*` to check.

To quit minicom, press `Ctrl-A` then `Q`.
