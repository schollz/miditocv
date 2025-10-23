# Copilot Instructions for miditocv

## Project Overview

miditocv is an embedded MIDI to CV converter built for the Raspberry Pi Pico. It combines embedded C/C++ for hardware control with Lua scripting for user-programmable behavior, plus a web-based code editor interface.

## Architecture

### Core Components

- **Embedded Firmware** (C/C++): Main firmware running on Raspberry Pi Pico RP2040
  - Hardware abstraction for DACs, ADCs, I2C, SPI, MIDI I/O
  - Lua VM integration for user scripting
  - USB MIDI and UART MIDI support
  - Real-time audio/CV generation

- **Lua Scripting Engine**: User-facing programming interface
  - Callback-based API (on_note_on, on_cc, on_beat, etc.)
  - Sequencing library (S/sequins)
  - CV/voltage conversion utilities
  - Runs embedded in the C firmware

- **Web Interface** (JavaScript/Vue.js): Online code editor
  - Located in `web/` directory
  - Built with Parcel bundler
  - Provides live code editing and testing
  - Hosted at https://my.miditocv.com

- **Calibration Tools** (Python): Hardware calibration utilities
  - Located in `dev/` directory
  - Uses NI DAQ hardware for voltage measurement
  - Windows-only (nidaqmx dependency)

## Build System

### Prerequisites

- CMake 3.12+
- ARM GCC toolchain (gcc-arm-none-eabi)
- Pico SDK v1.5.1
- Node.js (for web builds and luamin)
- Lua 5.4.6 (included in tree)
- xxd (for binary embedding)

### Build Commands

```bash
# Debug build
make miditocv-debug

# Release build
make miditocv-release

# Run tests
make test

# Build web interface
make publish
```

### Build Process

1. `lib/lua_globals.h` is auto-generated from `web/static/globals.lua` using luamin + xxd
2. CMake configures the build with Pico SDK integration
3. Generated .uf2 file can be uploaded to Pico via USB mass storage

## Testing

### C Tests
- Located in `lib/tests/lua/`
- Test Lua VM integration, panic handling, and core functionality
- Run with: `make test`
- Uses standard Lua C API for verification

### CI/CD
- **Build workflow**: Compiles firmware for every PR/push to main
- **CI workflow**: Runs C tests and builds web assets
- Both workflows are defined in `.github/workflows/`

## Code Style & Standards

### C/C++ Code

- C11 standard for C code
- C++17 for C++ code
- Use existing utility macros: `util_clamp`, `linlin`, `linexp`
- Follow Pico SDK conventions for hardware access
- Minimize warnings (warnings are not errors, but keep code clean)
- Memory-constrained environment - be mindful of allocations

### Lua Code

- Lua 5.4 syntax
- Global variables for state: `volts`, `trigger`, `gate`, `shift`, `button[]`
- Use callback functions: `on_beat()`, `on_knob()`, `on_button()`, `on_note_on()`, etc.
- Voltage range: -5V to 10V
- See README.md for complete Lua API documentation

### JavaScript/Web Code

- Modern JavaScript (ES6+)
- Vue.js 3 for UI components
- CodeMirror for code editing
- Keep dependencies minimal

## File Structure

### Key Directories

- `lib/` - Hardware abstraction, utilities, and embedded libraries
- `web/` - Web-based code editor and static assets
- `dev/` - Development tools, calibration scripts
- `examples/` - Example Lua programs
- `lua-5.4.6/` - Lua interpreter source (vendor)
- `build/` - CMake build output (gitignored)

### Important Files

- `main.c` - Main firmware entry point
- `CMakeLists.txt` - Build configuration
- `Makefile` - High-level build orchestration
- `lib/miditocv.h` - Main hardware interface
- `lib/luavm.h` - Lua VM integration
- `web/static/globals.lua` - Lua standard library additions (embedded in firmware)

## Hardware Configuration

### Platform: Raspberry Pi Pico (RP2040)

- Dual-core ARM Cortex-M0+
- 264KB SRAM
- 2MB Flash
- PIO state machines for timing-critical I/O

### Pin Definitions

Defined in `CMakeLists.txt`:
- MIDI_RX_PIN=9
- I2C0_SDA_PIN=4, I2C0_SCL_PIN=5
- I2C1_SDA_PIN=2, I2C1_SCL_PIN=3
- WS2812_PIN=7 (LED control)
- SD Card pins (SPI/SDIO)

### Peripherals

- DACs (MCP4725/MCP4728) for CV output
- ADCs for CV input
- MIDI UART + USB MIDI
- SD card for program storage
- WS2812 LEDs for UI feedback

## Common Tasks

### Adding a New Lua API Function

1. Implement C function in appropriate lib/*.h file
2. Register function in Lua VM (see `lib/luavm.h`)
3. Document in README.md under the API section
4. Add test in `lib/tests/lua/` if applicable
5. Update `web/static/globals.lua` if needed

### Modifying Web Interface

1. Edit files in `web/`
2. Test locally with `make web` (requires mkcert)
3. Build production bundle with `make publish`
4. Output goes to `web/dist/`

### Debugging Firmware

1. Build in debug mode: `make miditocv-debug`
2. Use OpenOCD + GDB (see README "Debugger" section)
3. Serial debugging available via minicom on /dev/ttyACM1

### Calibration

1. Windows only (requires NI DAQ hardware)
2. Run `dev/calibrate.py <ID>` with proper setup
3. See README "Calibration" section for full details

## Important Constraints

- **Memory**: RP2040 has limited RAM - avoid large allocations
- **Real-time**: Audio/CV generation is timing-critical
- **Flash**: Limited to 2MB including Lua programs
- **Lua**: VM is sandboxed, no file I/O except SD card access
- **Dependencies**: Minimize external dependencies to reduce binary size

## Contributing Guidelines

1. Test changes locally before committing
2. Ensure CI passes (run `make test` locally)
3. Update documentation for API changes
4. Follow existing code style and patterns
5. For firmware changes, verify on actual hardware if possible
6. Keep commits focused and atomic

## Additional Resources

- [Lua 5.4 Manual](https://www.lua.org/manual/5.4/manual.html)
- [Raspberry Pi Pico SDK Documentation](https://www.raspberrypi.com/documentation/microcontrollers/raspberry-pi-pico.html)
- [Project README](../README.md) - Complete API documentation
- Online editor: https://my.miditocv.com

## Notes for Code Generation

- When generating Lua code, follow the callback pattern shown in README
- For C code, use Pico SDK APIs rather than standard POSIX when possible
- Remember voltage output range is -5V to 10V (not 0-5V)
- Gate parameter is a fraction (0.0-1.0) of beat duration
- Triggers are edge-triggered (low-to-high transition)
- The `S` sequencing library (sequins) is available for pattern generation
