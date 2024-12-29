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

Here are examples of the functions.

### `on_beat(beat)`

This function is called every time a beat is detected. The `beat` is a number that increments every beat. You can return two numbers: the first is the voltage to set to the output and the second is a boolean of whether to trigger the output (if any envelopes or gates are linked to this). 

```lua
local envelope_trigger = false  
local volts = 0
function on_beat(beat)
    volts = volts + 0.1
    if volts > 5 then
        volts = 0
    end
    envelope_trigger = beat%2==0
    return volts, envelope_trigger
end
```

This function is special, in that it *can be linked to a clock output*, but if it is not linked to a clock output it will run at the tempo of the global clock.

### `on_note_on(channel,note,velocity)`, `on_note_off(channel,note)`

These functions expose the MIDI note on and note off events. The `channel` is the MIDI channel, the `note` is the MIDI note, and the `velocity` is the MIDI velocity.

```lua
function on_note_on(channel,note,velocity)
    return (note-60.0)/12.0, true
end

function on_note_off(channel,note)
    return 0, false
end
```

### `on_cc(channel,cc,value)`

This function exposes the MIDI CC events. The `channel` is the MIDI channel, the `cc` is the MIDI CC number, and the `value` is the MIDI value.

```lua
function on_cc(channel,cc,value)
    return value/127.0, true
end
```

### `on_key_pressure(channel,key,pressure)`

This function exposes the MIDI key pressure events. The `channel` is the MIDI channel, the `key` is the MIDI key, and the `pressure` is the MIDI pressure.

```lua
function on_key_pressure(channel,key,pressure)
    return pressure/127.0, true
end
```

### `on_channel_pressure(channel,pressure)`

This function exposes the MIDI channel pressure events. The `channel` is the MIDI channel, and the `pressure` is the MIDI pressure.

```lua
function on_channel_pressure(channel,pressure)
    return pressure/127.0, true
end
```

### `on_pitch_bend(channel,pitch)`

This function exposes the MIDI pitch bend events. The `channel` is the MIDI channel, and the `pitch` is the MIDI pitch.

```lua
function on_pitch_bend(channel,pitch)
    return pitch/16383.0, true
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
