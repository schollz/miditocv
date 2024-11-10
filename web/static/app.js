const { createApp, ref, computed, watch, onMounted } = Vue;
var vm;
let disableWatchers = false;
let scenes_updated = [false, false, false, false, false, false, false, false];
let last_time_of_message_received = 0;
function addToMidiConsole(message) {
    console.log('MIDI message:', message);
}

async function updateWithoutWatcher(scene_num, output_num, param, value) {
    disableWatchers = true;
    vm.scenes[scene_num].outputs[output_num][param] = value;
    await Vue.nextTick();
    disableWatchers = false;
}

async function updateLocalScene(scene_num) {
    if (scenes_updated[scene_num]) {
        return;
    }
    scenes_updated[scene_num] = true;
    disableWatchers = true;
    // get all parameters
    for (let output_num = 0; output_num < 8; output_num++) {
        for (let param of Object.keys(vm.scenes[scene_num].outputs[output_num])) {
            let value = vm.scenes[scene_num].outputs[output_num][param];
            sysex_string = `${scene_num}_${output_num}_${param.replace(/_/g, '')}_-10.0`;
            console.log(`[sending_sysex] ${sysex_string}`);
            send_sysex(sysex_string);
            await new Promise(resolve => setTimeout(resolve, 20));
            await Vue.nextTick();
        }
    }
    disableWatchers = false;
}

function setupMidiInputListener() {
    if (window.inputMidiDevice) {
        window.inputMidiDevice.onmidimessage = (midiMessage) => {
            // check if sysex
            // console.log(midiMessage.data);
            if (midiMessage.data[0] == 0xf0) {
                // convert the sysex to string 
                last_time_of_message_received = Date.now();
                var sysex = "";
                for (var i = 1; i < midiMessage.data.length - 1; i++) {
                    sysex += String.fromCharCode(midiMessage.data[i]);
                }
                fields = sysex.split(" ");
                // see if it starts with version=
                if (sysex.startsWith("v")) {
                    //console.log(`[version] ${sysex}`);
                    return;
                } else if (sysex.startsWith("scene")) {
                    // extract number from string
                    console.log(`[sysex_receieved] ${sysex}`);
                    let scene_num = Number(sysex.match(/\d+/)[0]);
                    // update the scene
                    vm.current_scene = scene_num;
                    updateLocalScene(scene_num);
                } else if (fields.length == 4) {
                    // check if field [3] is a parameter
                    console.log(`[sysex_receieved] ${fields[0]} ${fields[1]} ${fields[2]} ${fields[3]}`);
                    let scene_num = Number(fields[0]);
                    let output_num = Number(fields[1]);
                    if (vm.scenes[scene_num] && vm.scenes[scene_num].outputs[output_num]) {
                        let output = vm.scenes[scene_num].outputs[output_num];
                        let param = fields[2];
                        let value = Number(fields[3]);
                        if (output[param] != value) {
                            updateWithoutWatcher(scene_num, output_num, param, value);
                        }
                    }
                } else {
                    addToMidiConsole(sysex);
                }
            } else {
                addToMidiConsole(midiMessage.data);
            }
        };
    }
}

function setupMidi() {
    navigator.requestMIDIAccess({ sysex: true })
        .then((midiAccess) => {
            // Input setup
            const inputs = midiAccess.inputs.values();
            for (let input of inputs) {
                if (input.name.includes("yoctocore") || input.name.includes("zeptocore") || input.name.includes("ectocore")) {
                    window.inputMidiDevice = input;
                    setupMidiInputListener();
                    console.log("input device connected");
                    break;
                }
            }

            // Output setup
            const outputs = midiAccess.outputs.values();
            for (let output of outputs) {
                // console.log(output.name);
                if (output.name.includes("yoctocore") || output.name.includes("zeptocore") || output.name.includes("ectocore")) {
                    window.yoctocoreDevice = output;
                    console.log("output device connected");
                    sysex_string = `0_0_scene_-10.0`;
                    console.log(`[sending_sysex] ${sysex_string}`);
                    send_sysex(sysex_string);
                    vm.device_connected = true;
                    break;
                }
            }


        })
        .catch((error) => {
            console.error("Could not access MIDI devices.", error);
        });
}



function send_sysex(str) {
    if (window.yoctocoreDevice) {
        // Create a Uint8Array with start (0xF0) and end (0xF7) bytes
        const sysex = new Uint8Array(str.length + 2);
        sysex[0] = 0xF0;

        // Fill the array with the string data, ensuring all bytes are 7-bit safe
        for (let i = 0; i < str.length; i++) {
            const charCode = str.charCodeAt(i);

            // Ensure the character code is 7-bit (0x00 - 0x7F)
            sysex[i + 1] = charCode & 0x7F;
        }

        // Set the end byte (0xF7)
        sysex[str.length + 1] = 0xF7;

        // Send the SysEx message
        try {
            window.yoctocoreDevice.send(sysex);
            // console.log("SysEx message sent:", sysex);
        } catch (error) {
            console.error("Failed to send SysEx message:", error);
        }
    } else {
        console.warn("No MIDI device connected.");
    }
}

document.addEventListener('DOMContentLoaded', () => {
    // if chrome and on desktop
    if (window.chrome && window.chrome.app) {
        setupMidi();

        setTimeout(() => {
            setInterval(() => {
                if (Date.now() - last_time_of_message_received > 317 * 2) {
                    window.yoctocoreDevice && send_sysex("version0");
                }
                if (Date.now() - last_time_of_message_received > 317 * 4) {
                    vm.device_connected = false;
                    setupMidi();
                }
            }, 317);
        }, 2000);

    }

});

const app = createApp({
    setup() {
        // Initialize scenes and outputs
        const scenes = ref(
            Array.from({ length: 8 }, () => ({
                outputs: Array.from({ length: 8 }, () => ({
                    mode: 0,
                    min_voltage: -5,
                    max_voltage: 10,
                    slew_time: 0,
                    quantization: 0,
                    midi_channel: 0,
                    midi_cc: 0,
                    clock_tempo: 120,
                    clock_division: 1,
                    lfo_waveform: 0,
                    lfo_period: 1,
                    lfo_depth: 1,
                })),
            }))
        );
        const current_scene = ref(0);
        const current_output = ref(0);
        const selected_output = computed(() => {
            return scenes.value[current_scene.value].outputs[current_output.value];
        });
        const device_connected = ref(false);



        function getButtonClass(mode) {
            mode = Number(mode);
            switch (mode) {
                case 0:
                    return 'mode-manual';
                case 1:
                    return 'mode-midi-pitch';
                case 2:
                    return 'mode-midi-envelope';
                case 3:
                    return 'mode-midi-cc';
                case 4:
                    return 'mode-midi-clock';
                case 5:
                    return 'mode-clock';
                case 6:
                    return 'mode-lfo';
                default:
                    return '';
            }
        }

        const defaultValues = {
            0: {
                min_voltage: -5,
                max_voltage: 10,
                slew_time: 0,
                quantization: 0,
            },
            1: {
                min_voltage: -5,
                max_voltage: 10,
                slew_time: 0,
                quantization: 1,
                midi_channel: 0,
            },
            2: {
                min_voltage: 0,
                max_voltage: 10,
                midi_channel: 0,
            },
            3: {
                midi_channel: 0,
                midi_cc: 0,
            },
            4: {
                midi_channel: 0,
                min_voltage: 0,
                max_voltage: 5,
            },
            5: {
                clock_tempo: 120,
                clock_division: 1,
                min_voltage: 0,
                max_voltage: 5,
            },
            6: {
                lfo_waveform: 0,
                lfo_period: 1,
                lfo_depth: 1,
                min_voltage: -5,
                max_voltage: 5,
            },
        };

        // watch scene change 
        watch(
            () => current_scene.value,
            (newScene) => {
                // send sysex to update the scene
                updateLocalScene(newScene);
                // update the scene on the device
                sysex_string = `0_0_scene_${newScene}.0`;
                console.log(`[sending_sysex] ${sysex_string}`);
                send_sysex(sysex_string);
            }
        );

        watch(
            () => selected_output.value.max_voltage,
            (newValue) => {
                // If max_voltage exceeds 10, set it back to 10
                if (newValue > 10) {
                    selected_output.value.max_voltage = 10;
                }
            }
        );

        watch(
            () => selected_output.value.min_voltage,
            (newValue) => {
                // If min_voltage exceeds -5, set it back to -5
                if (newValue < -5) {
                    selected_output.value.min_voltage = -5;
                }
            }
        )

        // // Watcher for mode changes to update default values
        // watch(
        //     () => selected_output.value.mode,
        //     (newMode) => {
        //         const modeDefaults = defaultValues[newMode] || {};
        //         Object.assign(selected_output.value, modeDefaults);
        //     }
        // );

        // Debounce function
        function debounce(fn, delay) {
            let timeout;
            return (...args) => {
                clearTimeout(timeout);
                timeout = setTimeout(() => fn(...args), delay);
            };
        }

        // Create a map to store debounce timers for each output
        const debounceMap = new Map();

        // Helper function to get a unique key for each output
        function getDebounceKey(sceneIndex, outputIndex, property) {
            return `${sceneIndex}-${outputIndex}-${property}`;
        }

        // Function to log changes with per-output debouncing
        function logChange(sceneIndex, outputIndex, property, value) {
            const key = getDebounceKey(sceneIndex, outputIndex, property);

            // return if value is empty
            if (value === "") {
                return;
            }

            // Create a debounced logger if it doesn't exist
            if (!debounceMap.has(key)) {
                debounceMap.set(
                    key,
                    debounce((sceneIdx, outputIdx, prop, val) => {
                        val = Number(val);
                        sysex_string = `${sceneIdx}_${outputIdx}_${prop.replace(/_/g, '')}_${val.toPrecision(4)}`;
                        console.log(`[sending_sysex] ${sysex_string}`);
                        send_sysex(sysex_string);
                    }, 150)
                );
            }

            // Call the debounced logger with the current change
            debounceMap.get(key)(sceneIndex, outputIndex, property, value);
        }



        // Watcher with per-output debouncing
        watch(
            () => JSON.stringify(scenes.value),
            (newVal, oldVal) => {
                if (disableWatchers) {
                    return;
                }
                const parseNew = JSON.parse(newVal);
                const parseOld = JSON.parse(oldVal);

                // Iterate through each scene and output
                parseNew.forEach((scene, sceneIndex) => {
                    scene.outputs.forEach((output, outputIndex) => {
                        const oldOutput = parseOld[sceneIndex]?.outputs[outputIndex];

                        // Check for changes in each param
                        Object.keys(output).forEach((param) => {
                            if (output[param] !== oldOutput[param]) {
                                logChange(sceneIndex, outputIndex, param, output[param]);
                            }
                        });

                    });
                });
            },
            { deep: true }
        );

        // Functions to update the current scene and output
        const select_scene = (index) => {
            current_scene.value = index;
        };

        const select_output = (index) => {
            current_output.value = index;
        };

        return {
            scenes,
            current_scene,
            current_output,
            selected_output,
            select_scene,
            select_output,
            getButtonClass,
            device_connected,
        };
    },
});

vm = app.mount('#app');
