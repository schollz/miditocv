export PICO_EXTRAS_PATH ?= $(CURDIR)/pico-extras
export PICO_SDK_PATH ?= $(CURDIR)/pico-sdk
NPROCS := $(shell grep -c 'processor' /proc/cpuinfo)

GOVERSION = go1.21.11
GOBIN = $(HOME)/go/bin
GOINSTALLPATH = $(GOBIN)/$(GOVERSION)

yoctocore: pico-extras build 
	make -C build -j$(NPROCS)
	echo "build success"
	cp build/*.uf2 yoctocore.uf2

build: 
	mkdir -p build
	cd build && cmake ..

envs:
	export PICO_EXTRAS_PATH=/home/zns/pico/pico-extras 
	export PICO_SDK_PATH=/home/zns/pico/pico-sdk 

pico-extras:
	git clone https://github.com/raspberrypi/pico-extras.git pico-extras
	cd pico-extras && git checkout sdk-1.5.1
	cd pico-extras && git submodule update -i
	
changebaud:
	-curl localhost:7083 

resetpico2:
	-amidi -p $$(amidi -l | grep 'yoctocore\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	sleep 0.1
	-amidi -p $$(amidi -l | grep 'yoctocore\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	sleep 0.1
	-amidi -p $$(amidi -l | grep 'yoctocore\|zeptoboard\|ectocore' | awk '{print $$2}') -S "F0 64 69 73 6B 6D 6F 64 65 31 F7"
	sleep 0.1

upload: resetpico2 changebaud yoctocore
	./dev/upload.sh 

clean:
	rm -rf build
	rm -rf *.wav
	rm -rf lib/biquad.h

cloc:
	cloc --exclude-list-file=dev/.clocignore --exclude-lang="make,CMake,D,Markdown,JSON,INI,Bourne Shell,TOML,TypeScript,YAML,Assembly" *

ignore:
	git status --porcelain | grep '^??' | cut -c4- >> .gitignore
	git commit -am "update gitignore"


.PHONY: web
BROWSERSYNC_CMD = cd web && browsersync 
SSL_PROXY_CMD = cd web && local-ssl-proxy --key localhost-key.pem --cert localhost.pem --source 8000 --target 8003
web:
	@echo "Starting BrowserSync in the background..."
	@($(BROWSERSYNC_CMD) &) && \
	echo "Starting local SSL proxy..." && \
	$(SSL_PROXY_CMD)