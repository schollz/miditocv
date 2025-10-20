export PICO_EXTRAS_PATH ?= $(CURDIR)/pico-extras
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk
NPROCS := $(shell grep -c 'processor' /proc/cpuinfo)

GOVERSION = go1.21.11
GOBIN = $(HOME)/go/bin
GOINSTALLPATH = $(GOBIN)/$(GOVERSION)

miditocv-debug: lib/lua_globals.h pico-extras build
	make -C build -j$(NPROCS)
	echo "build success"
	cp build/*.uf2 miditocv.uf2

miditocv-release: lib/lua_globals.h pico-extras release
	make -C build -j$(NPROCS)
	echo "build success"
	cp build/*.uf2 miditocv.uf2

lib/lua_globals.h:	
	npm install -g luamin || true
	luamin --version || true
	xxd --version || true
	luamin -f web/static/globals.lua > globals.lua 
	xxd -i globals.lua > lib/lua_globals.h
	rm globals.lua

lua:
	lua web/static/globals.lua

lua-5.4.6/src/lua:
	cd lua-5.4.6 && make linux


test: lib/lua_globals.h lua-5.4.6/src/lua
	@echo "Running C test: test_panic_simple.c"
	@cd lib/tests/lua && gcc -o test_panic_simple test_panic_simple.c -I../../../lua-5.4.6/src ../../../lua-5.4.6/src/liblua.a -lm -ldl && ./test_panic_simple
	@echo ""
	@echo "Running C test: test_basic_simple.c"
	@cd lib/tests/lua && gcc -o test_basic_simple test_basic_simple.c -I../../../lua-5.4.6/src ../../../lua-5.4.6/src/liblua.a -lm -ldl && ./test_basic_simple

build:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Debug ..

release:
	mkdir -p build
	cd build && cmake -DCMAKE_BUILD_TYPE=Release ..

envs:
	export PICO_EXTRAS_PATH=/home/zns/pico/pico-extras
	export PICO_SDK_PATH=/home/zns/pico/pico-sdk

pico-extras:
	git clone https://github.com/raspberrypi/pico-extras.git pico-extras
	cd pico-extras && git checkout sdk-1.5.1
	cd pico-extras && git submodule update --init --recursive

changebaud:
	-curl localhost:7083

resetpico2:
	-amidi -p $$(amidi -l | grep 'miditocv\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	sleep 0.1
	-amidi -p $$(amidi -l | grep 'miditocv\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	sleep 0.1
	-amidi -p $$(amidi -l | grep 'miditocv\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	sleep 0.1

upload: resetpico2 changebaud miditocv-release
	./dev/upload.sh


clean:
	rm -rf build
	rm -rf *.wav
	rm -rf lib/biquad.h
	rm -rf lib/lua_globals.h

cloc:
	cloc --exclude-list-file=dev/.clocignore --exclude-lang="make,CMake,D,Markdown,JSON,INI,Bourne Shell,TOML,TypeScript,YAML,Assembly" *

ignore:
	git status --porcelain | grep '^??' | cut -c4- >> .gitignore
	git commit -am "update gitignore"

web/localhost.pem:
	go install -v filippo.io/mkcert@latest
	cd web && mkcert -install
	cd web && mkcert localhost

.PHONY: web
BROWSERSYNC_CMD = cd web && npm install && parcel index.html --no-source-maps
SSL_PROXY_CMD = cd web && local-ssl-proxy --key localhost-key.pem --cert localhost.pem --source 8000 --target 1234
web: web/localhost.pem
	-pkill -f -9 browsersync
	-pkill -f -9 browser-sync
	-pkill -f -9 local-ssl-proxy
	# npm install -g browser-sync local-ssl-proxy
	@echo "Starting BrowserSync in the background..."
	@($(BROWSERSYNC_CMD) &) && \
	echo "Starting local SSL proxy..." && \
	$(SSL_PROXY_CMD)


publish:
	# npm install --verbose -g npm@latest
	# npm install --verbose -g parcel@latest
	cd web && npm install
	cd web && rm -rf dist
	cd web && parcel build index.html --dist-dir ./dist --no-optimize
