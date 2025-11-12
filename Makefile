export PICO_EXTRAS_PATH ?= $(CURDIR)/pico-extras
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk
NPROCS := $(shell grep -c 'processor' /proc/cpuinfo)

GOVERSION = go1.21.11
GOBIN = $(HOME)/go/bin
GOINSTALLPATH = $(GOBIN)/$(GOVERSION)
PARCEL = $(CURDIR)/web/node_modules/.bin/parcel

# Default target
.DEFAULT_GOAL := miditocv-debug

# Phony targets (targets that don't create files)
.PHONY: all clean test lua web publish upload changebaud resetpico2 cloc ignore envs help
.PHONY: miditocv-debug miditocv-release release release-debug

# Main build targets
all: miditocv-debug

miditocv-debug: lib/lua_globals.h build/Makefile
	$(MAKE) -C build -j$(NPROCS)
	@echo "✅ Build success (debug)"
	cp build/*.uf2 miditocv.uf2

upload-debug: miditocv-debug
	cd build && sudo openocd -f interface/cmsis-dap.cfg -f target/rp2040.cfg -c "adapter speed 5000" -c "program miditocv.elf verify reset exit"

miditocv-release: lib/lua_globals.h build-release/Makefile
	$(MAKE) -C build-release -j$(NPROCS)
	@echo "✅ Build success (release)"
	cp build-release/*.uf2 miditocv.uf2

# CMake configuration targets (depend on pico-sdk and pico-extras)
build/Makefile: pico-sdk pico-extras CMakeLists.txt
	@echo "🔧 Configuring CMake (Debug)..."
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

build-release/Makefile: pico-sdk pico-extras CMakeLists.txt
	@echo "🔧 Configuring CMake (Release)..."
	mkdir -p build-release
	cd build-release && cmake -DCMAKE_BUILD_TYPE=Release ..

# Legacy aliases
release: build-release/Makefile

release-debug: build/Makefile

# Lua globals generation
.PHONY: lib/lua_globals.h
lib/lua_globals.h:
	@echo "🌀 Building lib/lua_globals.h..."
	@npm install -g luamin >/dev/null 2>&1 || true
	@echo "  Minifying web/static/globals.lua..."
	@if ! luamin -f web/static/globals.lua > globals.lua 2>/dev/null; then \
		echo "  ⚠️  luamin failed, using unminified source"; \
		cp web/static/globals.lua globals.lua; \
	fi
	@xxd -i globals.lua > lib/lua_globals.h
	@rm globals.lua
	@echo "  ✅ lib/lua_globals.h generated"

# Lua testing
lua:
	lua web/static/globals.lua

lua-5.4.6/src/lua:
	@echo "🔨 Building Lua 5.4.6..."
	$(MAKE) -C lua-5.4.6 linux

test: lib/lua_globals.h lua-5.4.6/src/lua
	@echo "🧪 Running Lua tests..."
	@echo "  → test_panic_simple.c"
	@cd lib/tests/lua && gcc -o test_panic_simple test_panic_simple.c -I../../../lua-5.4.6/src ../../../lua-5.4.6/src/liblua.a -lm -ldl && ./test_panic_simple
	@echo ""
	@echo "  → test_basic_simple.c"
	@cd lib/tests/lua && gcc -o test_basic_simple test_basic_simple.c -I../../../lua-5.4.6/src ../../../lua-5.4.6/src/liblua.a -lm -ldl && ./test_basic_simple
	@echo ""
	@echo "  → test_gate.c"
	@cd lib/tests/lua && gcc -o test_gate test_gate.c -I../../../lua-5.4.6/src ../../../lua-5.4.6/src/liblua.a -lm -ldl && ./test_gate
	@echo ""
	@echo "✅ All tests passed"

# Help target
help:
	@echo "Available targets:"
	@echo "  make                  - Build debug version (default)"
	@echo "  make all              - Build debug version"
	@echo "  make miditocv-debug   - Build debug version"
	@echo "  make miditocv-release - Build release version"
	@echo "  make test             - Run Lua tests"
	@echo "  make clean            - Remove build artifacts"
	@echo "  make upload           - Reset device and upload firmware"
	@echo "  make web              - Start web development server"
	@echo "  make publish          - Build web for production"
	@echo "  make pico-sdk         - Clone pico-sdk (if needed)"
	@echo "  make pico-extras      - Clone pico-extras (if needed)"
	@echo ""
	@echo "Environment variables:"
	@echo "  PICO_SDK_PATH=$(PICO_SDK_PATH)"
	@echo "  PICO_EXTRAS_PATH=$(PICO_EXTRAS_PATH)"


# Pico SDK and extras dependencies
pico-sdk:
	@echo "📦 Cloning pico-sdk..."
	git clone https://github.com/raspberrypi/pico-sdk
	cd pico-sdk && git checkout 1.5.1 && git submodule update --init --recursive
	@echo "✅ pico-sdk ready"

pico-extras:
	@echo "📦 Cloning pico-extras..."
	git clone https://github.com/raspberrypi/pico-extras.git pico-extras
	cd pico-extras && git checkout sdk-1.5.1 && git submodule update --init --recursive
	@echo "✅ pico-extras ready"

# Device utilities
changebaud:
	@echo "🔌 Changing baud rate..."
	-timeout 1 sudo minicom -b 1200 -o -D /dev/ttyACM0

resetpico2:
	@echo "🔄 Resetting Pico device..."
	-amidi -p $$(amidi -l | grep 'miditocv\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	@sleep 0.1
	-amidi -p $$(amidi -l | grep 'miditocv\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	@sleep 0.1
	-amidi -p $$(amidi -l | grep 'miditocv\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	@sleep 0.1
	@echo "✅ Reset complete"

upload: resetpico2 changebaud miditocv-release
	@echo "⬆️  Uploading firmware..."
	./dev/upload.sh
	@echo "✅ Upload complete"


clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf build build-release
	rm -rf *.wav *.uf2
	rm -rf lib/biquad.h lib/lua_globals.h
	@echo "✅ Clean complete"

# Development utilities
cloc:
	@echo "📊 Counting lines of code..."
	@cloc --exclude-list-file=dev/.clocignore --exclude-lang="make,CMake,D,Markdown,JSON,INI,Bourne Shell,TOML,TypeScript,YAML,Assembly" *

ignore:
	@echo "📝 Adding untracked files to .gitignore..."
	git status --porcelain | grep '^??' | cut -c4- >> .gitignore
	git commit -am "update gitignore"
	@echo "✅ .gitignore updated"

# Web development
$(PARCEL):
	@echo "📦 Installing Parcel..."
	cd web && npm install parcel
	@echo "✅ Parcel installed"

web/localhost.pem:
	@echo "🔐 Generating SSL certificates..."
	go install -v filippo.io/mkcert@latest
	cd web && mkcert -install
	cd web && mkcert localhost
	@echo "✅ SSL certificates ready"

BROWSERSYNC_CMD = cd web && $(PARCEL) index.html --no-source-maps
SSL_PROXY_CMD = cd web && local-ssl-proxy --key localhost-key.pem --cert localhost.pem --source 8000 --target 1234

web: web/localhost.pem $(PARCEL)
	@echo "🌐 Starting web development server..."
	-pkill -f -9 browsersync 2>/dev/null
	-pkill -f -9 browser-sync 2>/dev/null
	-pkill -f -9 local-ssl-proxy 2>/dev/null
	@echo "  Starting BrowserSync in the background..."
	@($(BROWSERSYNC_CMD) &) && \
	echo "  Starting local SSL proxy..." && \
	$(SSL_PROXY_CMD)

publish: $(PARCEL)
	@echo "📦 Building web for production..."
	cd web && npm install
	cd web && rm -rf dist
	cd web && $(PARCEL) build index.html --dist-dir ./dist --no-optimize
	@echo "✅ Web build complete (web/dist)"
