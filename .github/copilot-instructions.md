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

**ALWAYS install these before building:**
- CMake 3.12+ (`sudo apt install cmake`)
- ARM GCC toolchain: `sudo apt install gcc-arm-none-eabi libnewlib-arm-none-eabi`
- Build essentials: `sudo apt install build-essential gcc xxd`
- Node.js (latest) - for web builds and luamin
- Lua 5.4.6 (included in tree at `lua-5.4.6/`)
- Pico SDK v1.5.1 (see Pico SDK Setup below)

**CRITICAL: Pico SDK Setup**
```bash
# The build system expects PICO_SDK_PATH environment variable
export PICO_SDK_PATH=/path/to/pico-sdk

# For CI/GitHub Actions, Pico SDK is cloned at build time:
git clone https://github.com/raspberrypi/pico-sdk.git pico-sdk
cd pico-sdk && git checkout 1.5.1
git submodule update --init --recursive

# IMPORTANT: The build workflow patches tinyusb to prevent hangs:
sed -i 's/OSAL_TIMEOUT_WAIT_FOREVER/OSAL_TIMEOUT_NORMAL/g' pico-sdk/lib/tinyusb/src/tusb.c
```

### Build Commands & Validation

**ALWAYS run `make test` first** to verify the environment is set up correctly. Tests complete in under 30 seconds and verify:
- Lua VM is working
- Lua globals are compiled correctly
- Panic handling works

```bash
# 1. Run tests FIRST (always passes if environment is correct)
make test
# Expected: All tests pass (3 test files, ~45 assertions)
# Duration: ~30 seconds

# 2. Clean build (if needed)
make clean
# Removes: build/, lib/lua_globals.h, generated artifacts

# 3. Debug build (for development)
make miditocv-debug
# Creates: build/miditocv.uf2
# Duration: 2-3 minutes on first build, ~30s incremental
# Output: "build success" message, miditocv.uf2 in root

# 4. Release build (for production)
make miditocv-release
# Creates: build/miditocv.uf2 (optimized)
# Duration: 2-3 minutes on first build, ~30s incremental

# 5. Build web interface
make publish
# Creates: web/dist/ directory with bundled assets
# Duration: ~40 seconds
# Expected warnings: npm audit may show vulnerabilities (known, non-critical)
```

### Build Process Details

**Step-by-step build sequence:**

1. **Generate `lib/lua_globals.h`** (auto-run by make targets):
   - Minifies `web/static/globals.lua` using luamin (npm package)
   - Converts to C header with xxd
   - If luamin fails, falls back to unminified source
   - This file is gitignored and must be regenerated

2. **Build Lua 5.4.6** (for tests only):
   - `cd lua-5.4.6 && make linux`
   - Creates `lua-5.4.6/src/lua` and `lua-5.4.6/src/liblua.a`
   - Only needed for running C tests

3. **CMake Configuration**:
   - Creates `build/` directory
   - Configures with Debug or Release mode
   - Requires PICO_SDK_PATH to be set
   - Generates PIO headers for WS2812 and UART

4. **Compilation**:
   - Uses gcc-arm-none-eabi cross-compiler
   - Links with Pico SDK, Lua, FatFs, TinyUSB
   - Parallel build with make -j (uses all CPU cores)

5. **Output**:
   - `build/miditocv.elf` - ELF binary
   - `build/miditocv.uf2` - Bootloader format for Pico
   - Copied to root as `miditocv.uf2`

## Testing

### C Tests - ALWAYS RUN BEFORE CHANGES
- Located in `lib/tests/lua/`
- Test Lua VM integration, panic handling, gate behavior
- **ALWAYS run with: `make test`** (takes ~30 seconds)
- Tests compile and run automatically
- Three test files:
  - `test_panic_simple.c` - Panic detection and recovery
  - `test_basic_simple.c` - Basic Lua VM functionality
  - `test_gate.c` - Gate parameter behavior (39 assertions)

**Expected output:**
```
Running C test: test_panic_simple.c
=== Simulating Lua Panic Detection ===
...
✓ All tests passed!
```

**If tests fail:**
- Check that Lua 5.4.6 built correctly: `lua-5.4.6/src/lua --version`
- Check that lib/lua_globals.h exists (auto-generated)
- Ensure luamin is installed: `npm install -g luamin`

### CI/CD Workflows

**Build Workflow** (`.github/workflows/build.yml`):
- Runs on: Every PR and push to main
- Duration: ~3-4 minutes
- Steps:
  1. Install Pico SDK dependencies
  2. Clone Pico SDK v1.5.1 with submodules
  3. Patch tinyusb (critical for USB stability)
  4. Install luamin globally
  5. Run `make miditocv-release`
- **If this fails:** Check that Pico SDK is at v1.5.1 and tinyusb patch is applied

**CI Workflow** (`.github/workflows/ci.yml`):
- Runs on: Every PR and push to main
- Duration: ~2-3 minutes
- Two parallel jobs:
  1. **test**: Builds Lua 5.4.6, runs `make test`
  2. **publish**: Builds web assets with `make publish`
- **If tests fail:** Usually indicates Lua globals generation issue
- **If publish fails:** Usually npm dependency issue (check package-lock.json)

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
  - Core headers: `miditocv.h`, `luavm.h`, `dac.h`, `adsr.h`
  - Hardware drivers: `mcp4725.h`, `mcp4728.h`, `mcp3208.h`
  - MIDI handling: `libmidi.c`, `midi_comm.h`, `midiuart.h`
  - Tests: `lib/tests/lua/` - C test files for Lua VM
  - SD card: `lib/sdio/` - FatFs and SDIO implementation
- `web/` - Web-based code editor and static assets
  - `index.html` - Main editor page
  - `index.js` - Editor logic
  - `package.json` - Node dependencies (Parcel, CodeMirror, etc.)
  - `static/globals.lua` - Lua stdlib that gets embedded in firmware
- `dev/` - Development tools, calibration scripts
  - `calibrate.py` - Voltage calibration tool (Windows only)
  - `upload.sh` - Upload script for Linux
- `examples/` - Example Lua programs
  - `gate_example.lua` - Demonstrates gate parameter usage
- `lua-5.4.6/` - Lua interpreter source (vendor, build for tests only)
- `build/` - CMake build output (gitignored, created by make)
- `docs/` - Design documentation
  - `PANIC_HANDLING.md` - Lua panic recovery implementation
  - `PANIC_FLOW.md` - Panic handling flowcharts
- `docs2/` - Docusaurus documentation site (separate project)

### Important Files

- `main.c` - Main firmware entry point (1600+ lines)
  - Hardware initialization
  - Main loop and callbacks
  - MIDI message handling
  - Button/knob processing
  - LED control
- `CMakeLists.txt` - Build configuration
  - Pin definitions
  - Compiler flags
  - Library linkage
  - PIO generation
- `Makefile` - High-level build orchestration
  - Targets: miditocv-debug, miditocv-release, test, publish, clean
  - Handles pico-extras checkout
  - Manages lua_globals.h generation
- `lib/miditocv.h` - Main hardware interface
  - Mode definitions (NOTE, ENVELOPE, GATE, CC, CLOCK, LFO, CODE, etc.)
  - Config structure with all parameters
  - Output (Out) structure
  - Yoctocore main structure
- `lib/luavm.h` - Lua VM integration
  - Lua state management
  - Callback functions (on_beat, on_knob, on_button, on_note_on, etc.)
  - Environment management (8 separate Lua environments)
  - Panic handling
- `web/static/globals.lua` - Lua standard library additions
  - Sequins (S) library for sequencing
  - Helper functions (er, to_cv, linlin)
  - Gets embedded in firmware as `lib/lua_globals.h`
- `lib/lua_globals.h` - **AUTO-GENERATED** (never edit directly)
  - Generated from `web/static/globals.lua`
  - Embedded in firmware
  - Recreated on every build
- `.gitignore` - Excludes build artifacts, node_modules, pico-extras, etc.

### Root Directory Contents

```
.github/          - GitHub Actions workflows and copilot instructions
CMakeLists.txt    - CMake build configuration
LICENSE           - Project license
Makefile          - High-level build commands
README.md         - User documentation with API reference
main.c            - Firmware entry point
pico_sdk_import.cmake - Pico SDK integration
pico_extras_import.cmake - Pico extras integration
requirements.txt  - Python dependencies (for calibration)
tusb_config.h     - TinyUSB configuration
testing.lua       - Ad-hoc test script
```

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
6. **ALWAYS run `make test` to verify**

### Modifying Web Interface

1. Edit files in `web/`
2. Test locally with `make web` (requires mkcert for HTTPS)
   - Installs local certificates
   - Starts BrowserSync on port 1234
   - Proxies through local-ssl-proxy on port 8000
3. Build production bundle with `make publish` (~40 seconds)
4. Output goes to `web/dist/`
5. **No tests required for web changes** (unless modifying Lua globals)

### Making Firmware Changes

**CRITICAL WORKFLOW:**
1. Run `make test` BEFORE making changes to establish baseline
2. Make your changes to C/C++ files
3. Run `make test` to verify no regressions
4. Build with `make miditocv-debug` or `make miditocv-release`
5. If build fails, check:
   - PICO_SDK_PATH is set correctly
   - All Pico SDK submodules are initialized
   - lib/lua_globals.h exists (auto-generated)

**Common pitfalls:**
- Modifying `lib/lua_globals.h` directly (it's auto-generated!)
- Not running tests before committing
- Forgetting to patch tinyusb for USB stability
- Not setting PICO_SDK_PATH environment variable

### Debugging Firmware

1. Build in debug mode: `make miditocv-debug`
2. Use OpenOCD + GDB (see README "Debugger" section)
   - Requires: `sudo apt install gdb-multiarch openocd minicom`
   - Upload: `sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program miditocv.elf verify reset exit"`
   - Debug: `gdb-multiarch miditocv.elf -ex "target remote localhost:3333" -ex "monitor reset init" -ex "continue"`
3. Serial debugging available via minicom on /dev/ttyACM1
   - Quit with Ctrl-A then Q
   - Baud rate: 115200

### Modifying Lua Standard Library

**IMPORTANT: Changes to globals.lua affect ALL Lua code!**

1. Edit `web/static/globals.lua`
2. Run `make clean` (removes cached lua_globals.h)
3. Run `make test` (regenerates lua_globals.h and tests)
4. If tests pass, rebuild firmware
5. Test on actual hardware if possible

### Calibration

1. **Windows only** (requires NI DAQ hardware)
2. Setup: `cd dev && uv venv .venv --python=python3.12`
3. Activate: `.venv\Scripts\activate` (Windows)
4. Install: `uv pip install -r requirements.txt`
5. Run: `python calibrate.py <ID>`
6. See README "Calibration" section for full details
7. Calibration data stored in `dev/calibrations/`

## Important Constraints

- **Memory**: RP2040 has limited RAM (264KB) - avoid large allocations
- **Real-time**: Audio/CV generation is timing-critical - avoid blocking operations
- **Flash**: Limited to 2MB including Lua programs - keep firmware size minimal
- **Lua**: VM is sandboxed, no file I/O except SD card access
- **Dependencies**: Minimize external dependencies to reduce binary size
- **Build artifacts**: Never commit `build/`, `lib/lua_globals.h`, `web/dist/`, `node_modules/`

## Known Issues & Workarounds

### TinyUSB Hang Issue
**Problem:** TinyUSB can hang waiting for mutexes indefinitely
**Solution:** Applied in CI/CD workflows:
```bash
sed -i 's/OSAL_TIMEOUT_WAIT_FOREVER/OSAL_TIMEOUT_NORMAL/g' pico-sdk/lib/tinyusb/src/tusb.c
```
**When:** ALWAYS apply this patch after cloning Pico SDK

### Luamin Failure
**Problem:** luamin sometimes fails to minify Lua code
**Solution:** Makefile automatically falls back to unminified source
**Impact:** Slightly larger firmware, but functionally identical

### NPM Audit Vulnerabilities
**Problem:** `npm audit` shows vulnerabilities in web dependencies
**Status:** Known, non-critical (dev dependencies only)
**Action:** Safe to ignore unless specifically addressing security issues

### Pico SDK Version
**Critical:** MUST use Pico SDK v1.5.1
**Problem:** Other versions may have incompatibilities
**Solution:** Always checkout tag 1.5.1 and init submodules

### Missing PICO_SDK_PATH
**Problem:** Build fails with "PICO SDK location was not specified"
**Solution:** 
```bash
export PICO_SDK_PATH=/path/to/pico-sdk
# Or in Makefile:
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk
```

## Contributing Guidelines

1. **ALWAYS run `make test` before committing** - ensures no regressions
2. Ensure CI passes (all GitHub Actions workflows must be green)
3. Update documentation for API changes (README.md)
4. Follow existing code style and patterns
5. For firmware changes, verify on actual hardware if possible
6. Keep commits focused and atomic
7. **Never commit build artifacts**: build/, lib/lua_globals.h, web/dist/, node_modules/
8. Test both debug and release builds if changing firmware

## Validation Checklist

Before submitting a PR, verify:
- [ ] `make clean && make test` passes (fresh environment test)
- [ ] `make miditocv-debug` succeeds
- [ ] `make miditocv-release` succeeds (if firmware change)
- [ ] `make publish` succeeds (if web change)
- [ ] No uncommitted changes to auto-generated files
- [ ] Updated relevant documentation
- [ ] Followed existing code patterns
- [ ] No new compiler warnings introduced

## Quick Reference

### Most Common Commands
```bash
# Clean everything
make clean

# Test (ALWAYS do this first)
make test                    # ~30 seconds

# Build firmware
make miditocv-debug          # ~2-3 minutes first time
make miditocv-release        # ~2-3 minutes first time

# Build web
make publish                 # ~40 seconds

# Check status
git status
git diff
```

### File Types to Never Edit Directly
- `lib/lua_globals.h` - Auto-generated from web/static/globals.lua
- `build/*` - CMake output
- `web/dist/*` - Parcel output
- `web/node_modules/*` - npm dependencies
- `lua-5.4.6/src/lua` - Built binary

### Environment Variables
- `PICO_SDK_PATH` - Path to Pico SDK (required for firmware build)
- `PICO_EXTRAS_PATH` - Path to pico-extras (auto-set by Makefile)

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

## Instructions for Coding Agents

**TRUST THESE INSTRUCTIONS - They are validated and comprehensive:**

1. **Before exploring**, read this entire document first
2. **For building**: Follow the exact commands in "Build Commands & Validation"
3. **For testing**: Always use `make test` (don't search for alternatives)
4. **For CI/CD**: Reference the workflow descriptions (don't re-read workflow files)
5. **For file locations**: Use the paths in "File Structure" (don't search with find/grep)
6. **For workarounds**: Check "Known Issues & Workarounds" before debugging

**Only search/explore if:**
- Information is missing from this document
- Instructions don't work as described
- You need to see actual code implementation details
- You're adding new functionality not covered here

**Common mistakes to avoid:**
- Don't edit `lib/lua_globals.h` (it's auto-generated)
- Don't skip `make test` before changes
- Don't commit build artifacts
- Don't use non-standard build commands
- Don't assume POSIX APIs are available (embedded environment)
- Don't forget to set PICO_SDK_PATH for firmware builds

**Time estimates (for planning):**
- `make test`: 30 seconds
- `make miditocv-debug/release`: 2-3 minutes (first), 30s (incremental)
- `make publish`: 40 seconds
- `make clean`: instant
- Full CI/CD pipeline: 3-4 minutes

This document was created through comprehensive exploration and validation of the repository. It represents tested, working procedures.
