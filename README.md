# yoctocore

there are eight scenes. each scene is selected by holding down `shift` and then holding one of the eight buttons for >1 second.

each scene has eight outputs. each output can be configured to a different mode. modes are selected holding `shift` and pressing one of the eight buttons.

| Category  | Button        | Knob          | Shift + Button | Shift + Knob  |
|-----------|---------------|---------------|----------------|---------------|
| Manual    |               | voltage       | reset to zero  | slew          |
| Pitch     |               | voltage       | reset to zero  | portamento    |
| Envelope  | trigger       | attack        |                | release       |
| CC        |               |               | listen         |               |
| Clock     | tap tempo     | bpm           |                | division      |
| LFO       | reset         | period        |                | shape         |
| Code      | key(on,shift) | knob(x,shift) | key(on,shift)  | knob(x,shift) |
| Sequencer | jump here     | voltage       |                | duration      |

## TODO

- add code mode, Lua + fengari

## modes

### manual

[manual mode](#manual) is the default mode. the output voltage is controlled by the knob.

the output range is configurable in the editor, the default range is -5v to 10v.

the output voltage quantization is configurable in the editor, the default quantization is the resolution of the knob (12-bit) scaled by the output range (minimum resolution is 0.004v).

the slew rate is configurable in the editor, the default slew rate is 12v/s.  the slew can also be configured by holding shift and turning the knob.

### midi pitch

[midi pitch mode](#midi-pitch) is a midi to CV converter. the output voltage is controlled by the midi note number. the output voltage is calculated as `(note - root) / 12` where `note` is the midi note number and `root` is the root note number. the root note number is configurable in the editor, the default root note number is 69 (middle A).

the output voltage quantization is configurable in the editor.

the MIDI channel is configurable in the editor, the default MIDI channel is 0 (any).

the knob can be used to manually change the current pitch, similar to [manual mode](#manual).

### midi envelope

[midi envelope mode](#midi-envelope) will trigger an ADSR envelope when a `note on` message is received and released when a `note off` message is received. the output voltage is controlled by the envelope parameters.

the attack, decay, sustain, and release times are configurable in the editor, the default times are 0.01s, 0.1s, 0.5, and 0.1s respectively.

the MIDI channel is configurable in the editor, the default MIDI channel is 0 (any).

on the device, the envelope parameters can be configured by holding shift and turning the knob. the knob will cycle through the parameters. pressing the button will trigger an envelope.

### midi cc

[midi cc mode](#midi-cc) is a midi to CV converter. the output voltage is controlled by the midi cc value. the output voltage is calculated as `cc / 127`.

the MIDI cc to listen for is configurable in the editor, the default cc is 0.

the MIDI channel is configurable in the editor, the default MIDI channel is 0 (any).

the output voltage range is configurable in the editor, the default range is -5v to 10v.

### clock

[clock mode](#clock) will output a clock signal. the clock signal is a square wave with a 50% duty cycle.

the default clock bpm can be set in the editor, the default bpm is 120. the clock bpm can also be configured by turning the knob. the middle knob position is always the default bpm.

the clock division can be configured in the editor, the default division is 1/4. the clock division can also be configured by holding shift and turning the knob.

the button can be used to manually create a tempo.

### lfo

[lfo mode](#lfo) will output a low frequency oscillator.

the lfo waveform can be configured in the editor, the default waveform is a sine wave. the lfo waveform can be changed by pressing the button.

the lfo frequency can be configured in the editor, the default frequency is 1hz. the lfo frequency can also be configured by turning the knob.

the lfo voltage range is configurable in the editor, the default range is -5v to 5v. the lfo range can be attenuated by holding shift and turning the knob.


## Calibration

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
