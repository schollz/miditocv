# yoctocore


[![CI/CD](https://github.com/schollz/yoctocore/actions/workflows/build.yml/badge.svg)](https://github.com/schollz/yoctocore/actions/workflows/build.yml) [![Latest](https://img.shields.io/github/v/release/schollz/yoctocore?color=brightgreen&label=Release)](https://github.com/schollz/yoctocore/releases/latest)

there are eight scenes. each scene is selected by holding down `shift` and then holding one of the eight buttons for >1 second.

each scene has eight outputs. each output can be configured to a different mode. modes are selected holding `shift` and pressing one of the eight buttons.

| Category         | Button        | Knob          | Shift + Button | Shift + Knob  | Button + Knob |
| ---------------- | ------------- | ------------- | -------------- | ------------- | ------------- |
| Note             |               | voltage       | tuning note    | change tuning | portamento    |
| Envelope         | trigger       | attack        |                | release       |               |
| Gate             | trigger       | duration      |                | probability   |               |
| CC               |               |               | listen         |               |               |
| Program change   |               |               | listen         |               |               |
| Key Pressure     |               |               | listen         |               |               |
| Channel Pressure |               |               | listen         |               |               |
| Pitch bend       |               |               | listen         |               |               |
| Clock            | start/stop    | tempo         | tap tempo      | division      |               |
| LFO              | pause         | period        | max voltage    | shape         |               |
| Code             | key(on,shift) | knob(x,shift) | key(on,shift)  | knob(x,shift) |               |

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
