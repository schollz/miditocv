# yoctocore


[![CI/CD](https://github.com/schollz/yoctocore/actions/workflows/build.yml/badge.svg)](https://github.com/schollz/yoctocore/actions/workflows/build.yml) [![Latest](https://img.shields.io/github/v/release/schollz/yoctocore?color=brightgreen&label=Release)](https://github.com/schollz/yoctocore/releases/latest)

there are eight scenes. each scene is selected by holding down `shift` and then holding one of the eight buttons for >1 second.

each scene has eight outputs. each output can be configured to a different mode. modes are selected holding `shift` and pressing one of the eight buttons.

The **bold** things are implemented.

| Category         | Button        | Knob          | Shift + Button  | Shift + Knob      | Button + Knob |
| ---------------- | ------------- | ------------- | --------------- | ----------------- | ------------- |
| Note             |               |               | **tuning note** | **change tuning** | **voltage**   |
| Envelope         | **trigger**   | **attack**    |                 | **sustain**       | **release**   |
| Gate             | **trigger**   |               |                 | probability       |               |
| CC               | **listen**    |               |                 |                   |               |
| Program change   | **listen**    |               |                 |                   |               |
| Key Pressure     | **listen**    |               |                 |                   |               |
| Channel Pressure | **listen**    |               |                 |                   |               |
| Pitch bend       | **listen**    |               |                 |                   |               |
| Clock            | start/stop    | tempo         | tap tempo       | division          |               |
| LFO              | pause         | period        | max voltage     | shape             |               |
| Code             | key(on,shift) | knob(x,shift) | key(on,shift)   | knob(x,shift)     |               |

## Code

The yoctocore is programmable with the [Lua language](https://www.lua.org/manual/5.4/manual.html), a powerful, efficient, lightweight, embeddable scripting language. 

The yoctocore has [an online editor](https://my.yoctocore.com) that you can use to test out your programs.

Yoctocore programs are based around a set of [callback functions](https://en.wikipedia.org/wiki/Callback_(computer_programming)) that are run on specific routines.

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


Here are examples of the functions.

### `on_beat(beat)`

This function is called every time a beat is detected. The `beat` is a number that increments every beat. You can return two numbers: the first is the voltage to set to the output and the second is a boolean of whether to trigger the output (if any envelopes or gates are linked to this). 

```lua
bpm = 120 -- define bpm
-- local variables
local envelope_trigger = false  
local voltage_increase = 0
function on_beat(beat)
    voltage_increase = voltage_increase + 0.1
    if voltage_increase > 10 then
        voltage_increase = 0
    end
    volts = voltage_increase
    trigger = beat%2==0
end
```

This function is special, in that it *can be linked to a clock output*, but if it is not linked to a clock output it will run at the tempo defined by the `bpm` variable.

### `on_knob(value,shift,button)`

This function exposes the knob events. The `value` is the knob value (0-1023), the `shift` is a boolean of whether the shift button is pressed, and the `button` is the boolean of whether the button next to the knob is pressed.

```lua
function on_knob(value,shift,button)
    if shift then 
        volts = value/1023.0
    else
        volts = value/1023.0 * 5.0
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
