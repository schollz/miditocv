# yoctocore


[![CI/CD](https://github.com/schollz/yoctocore/actions/workflows/build.yml/badge.svg)](https://github.com/schollz/yoctocore/actions/workflows/build.yml) [![Latest](https://img.shields.io/github/v/release/schollz/yoctocore?color=brightgreen&label=Release)](https://github.com/schollz/yoctocore/releases/latest)

there are eight scenes. each scene is selected by holding down `shift` and then holding one of the eight buttons for >1 second.

each scene has eight outputs. each output can be configured to a different mode. modes are selected holding `shift` and pressing one of the eight buttons.

The **bold** things are implemented.

| Category         | Button         | Knob          | Shift + Button  | Shift + Knob      | Button + Knob |
| ---------------- | -------------- | ------------- | --------------- | ----------------- | ------------- |
| Note             | **listen**     |               | **tuning note** | **change tuning** | **voltage**   |
| Envelope         | **trigger**    | **attack**    |                 | **sustain**       | **release**   |
| Gate             | **trigger**    |               |                 | **probability**   |               |
| CC               | **listen**     |               |                 |                   |               |
| Program change   | **listen**     |               |                 |                   |               |
| Key Pressure     | **listen**     |               |                 |                   |               |
| Channel Pressure | **listen**     |               |                 |                   |               |
| Pitch bend       | **listen**     |               |                 |                   |               |
| Clock            | **start/stop** |               | **tap tempo**   | **division**      | **tempo**     |
| LFO              |                | **period**    | **pause**       | **max voltage**   | **shape**     |
| Code             | key(on,shift)  | knob(x,shift) | key(on,shift)   | knob(x,shift)     |               |

## Code

The yoctocore is programmable with the [Lua language](https://www.lua.org/manual/5.4/manual.html), a powerful, efficient, lightweight, embeddable scripting language. 

It is currently under development. Here is the implementation status:

- [x] [`volts` and `trigger`](#volts-and-trigger)
- [x] [`on_beat(on)`](#on_beaton)
- [x] [`on_knob(value,shift)`](#on_knobvalueshift)
- [x] [`on_button(value,shift)`](#on_buttonvalueshift)
- [ ] [`on_note_on(channel,note,velocity)`](#on_note_onchannelnotevelocity)
- [ ] [`on_note_off(channel,note)`](#on_note_offchannelnote)
- [ ] [`on_cc(channel,cc,value)`](#on_ccchannelccvalue)
- [ ] [`on_key_pressure(channel,key,pressure)`](#on_key_pressurechannelkeypressure)
- [ ] [`on_channel_pressure(channel,pressure)`](#on_channel_pressurechannelpressure)
- [ ] [`on_pitch_bend(channel,pitch)`](#on_pitch_bendchannelpitch)
- [x] [`er(k,n,w)`](#erknw)
- [x] [`to_cv(value)`](#to_cvvalue)
- [x] [`S` - A Minimal Sequencing Library](#s---a-minimal-sequencing-library)

The yoctocore has [an online editor](https://my.yoctocore.com) that you can use to test out your programs.

Yoctocore programs are based around a set of [callback functions](https://en.wikipedia.org/wiki/Callback_(computer_programming)) that are run on specific routines.

### `volts` and `trigger`

For any function you can set the voltage, or you can raise a trigger. The voltage is a number between -5 and 10, and the trigger is a boolean. The trigger is used if any other events are linked. To set the voltage you set the global variable `volts`:

```lua
volts = 5
```

To set the trigger you set the global variable `trigger`:

```lua
trigger = true
```

The trigger will get reset to `false` after the trigger is used.

You can also return values from the functions, which is useful in debugging as it will printed in the online code editor.


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

This function is special, in that it *can be linked to a clock output*, but if it is not linked to a clock output it will run at the tempo defined by the `bpm` variable.

### `on_knob(value,shift)`

This function exposes the knob events. The `value` is the knob value (0.0-1.0), the `shift` is a boolean of whether the shift button is pressed.

```lua
function on_knob(value,shift)
    if shift then
        volts = value
    else
        volts = value * 5.0
    end
end
```

### `on_button(value,shift)`

This function exposes the button event. The `value` is `true` if the button is pressed, else `false`, and the `shift` is a boolean of whether the shift button is pressed.

```lua
function on_button(value,shift)
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

*Note:* This works on windows only.

The yoctocore core is calibrated using an NI USB-6009 device to read in the voltages while manipulating the raw values to obtain a calibration curve for adjusting the final voltage.

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

### Pre-requisites

```bash
sudo apt install gdb-multiarch openocd minicom
```

**uploading:**

(Make sure to make in debug mode).

```bash
cd build && make && sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program yoctocore.elf verify reset exit"
```

**setup server:**
```bash
sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000"
```

**continue:**
```bash
gdb-multiarch yoctocore.elf  -ex "target remote localhost:3333" -ex "monitor reset init" -ex "continue"
```

```
step
```

```
backtrace
```

**minicom**:
quit with `Ctrl-A` and then `Q`

```bash
minicom -b 115200 -o -D /dev/ttyACM1 
```