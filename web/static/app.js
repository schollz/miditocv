const { createApp, ref, computed, watch, onMounted, reactive } = Vue;
var vm;
var outputCodeMirror;
var myCodeMirror;

let disableWatchers = false;
let scenes_updated = [false, false, false, false, false, false, false, false];
let last_time_of_message_received = 0;
var midiInputs = {};
// create an array of 8 arrays for codeTexts
let codeTexts = [];
for (let i = 0; i < 8; i++) {
    codeTexts.push([]);
    for (let j = 0; j < 8; j++) {
        codeTexts[i].push("");
    }
}


function addToMidiConsole(message) {
    console.log('MIDI message:', message);
}

let resolveExternalTrigger; // This will hold the resolve function for the external trigger promise

function assert(condition, message) {
    if (!condition) {
        throw message || "Assertion failed";
    }
}

function drawSparkline(index, data, mode) {
    const canvas = document.getElementById(`sparkline-${index}`);
    if (!canvas) return;

    const ctx = canvas.getContext("2d");

    // Resize canvas to fit parent
    const parent = canvas.parentElement;
    canvas.width = parent.offsetWidth;
    canvas.height = 35; // Fixed height

    // Clear the canvas
    ctx.clearRect(0, 0, canvas.width, canvas.height);

    // Constants for scaling
    const minValue = -5.5;
    const maxValue = 10.5;
    const valueRange = maxValue - minValue;

    // Define colors based on index
    const indexColors = [
        "#000",          // index 0: mode-manual
        "#FF0000",     // index 1: mode-midi-pitch
        "#FFAA33",     // index 2: mode-midi-envelope
        "#FFEA00",     // index 3: mode-midi-cc
        "#32CD32",     // index 4: mode-midi-clock
        "#7DF9FF",     // index 5: mode-clock
        "#0096FF",     // index 6: mode-lfo
        "#CBC3E3",     // index 7: mode-sequencer
    ];

    // Get the color for the current index, fallback to black if index is invalid
    const color = indexColors[mode] || "#000";

    // Draw sparkline
    const step = canvas.width / (data.length - 1);
    ctx.beginPath();
    ctx.strokeStyle = color; // Use index-specific color
    ctx.lineWidth = 2.5;

    data.forEach((value, i) => {
        const x = i * step;

        // Scale the value from -5 to +10 range to canvas height
        const normalizedValue = (value - minValue) / valueRange; // Normalize to 0-1
        const y = canvas.height - normalizedValue * canvas.height; // Invert for canvas y-axis

        i === 0 ? ctx.moveTo(x, y) : ctx.lineTo(x, y);
    });

    ctx.stroke();
}


function generateSparklineData() {
    return Array.from({ length: 20 }, () => Math.random()); // Replace with real data
}

function hash_djb(str) {
    let hash = 5381; // Initialize hash value as in the C code
    let i = 0; // Iterator for the string

    while (i < str.length) {
        let c = str.charCodeAt(i++); // Get ASCII value of character and increment iterator
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    hash = hash >>> 0;
    return hash;
}

assert(hash_djb("helloworld") == 4294815105);
assert(hash_djb("hello") == 261238937);
assert(hash_djb("world") == 279393645);


function parseStructAndHashVariables(cCode) {
    // Match all variable names inside the struct using a regular expression
    const structPattern = /typedef\s+struct\s+\w*\s*{([^}]*)}/s;
    const match = cCode.match(structPattern);

    if (!match) {
        console.error("No struct found in the provided C code.");
        return;
    }

    // Extract the content of the struct
    const structBody = match[1];

    // Match all variable declarations inside the struct
    const variablePattern = /\s*\w+\s+(\w+)\s*;/g;
    let variables = [];
    let result;

    while ((result = variablePattern.exec(structBody)) !== null) {
        variables.push(result[1]); // Collect variable names
    }

    // Generate hashes for each variable name
    const variableHashes = variables.map(varName => ({
        variable: varName,
        hash: hash_djb(varName),
    }));

    // Print out the hashes
    console.log("Variable hashes:");
    variableHashes.forEach(entry => {
        console.log(`#define PARAM_${entry.variable.toLocaleUpperCase()} ${entry.hash}`);
    });

    return variableHashes;
}

// Example usage
const cCode = `
typedef struct Config {
  uint8_t mode;
  float min_voltage;
  float max_voltage;
  float slew_time;
  float v_oct;
  uint8_t root_note;
  uint16_t quantization;
  float portamento;
  uint8_t midi_channel;
  uint8_t midi_priority_channel;
  uint8_t midi_cc;
  float clock_tempo;
  uint8_t clock_division;
  float lfo_period;
  float lfo_depth;
  uint8_t lfo_waveform;
  float attack;
  float decay;
  float sustain;
  float release;
  uint8_t linked_to;
  uint8_t probability;
  bool sparkline_do_update;
  char code;
  uint15_t code_len;
  float duration;
  float setpoint_voltage;
} Config;

`;

parseStructAndHashVariables(cCode);

// Function that simulates an external trigger
function externalTrigger() {
    if (resolveExternalTrigger) {
        resolveExternalTrigger(); // Trigger the promise when called
        resolveExternalTrigger = null; // Reset to avoid double triggering
    }
}

// Function that waits for the external trigger with a maximum wait time
function waitForTriggerOrTimeout(maxWaitTime) {
    return new Promise((resolve, reject) => {
        // Create a new promise for the external trigger
        const externalPromise = new Promise(resolveTrigger => {
            resolveExternalTrigger = resolveTrigger; // Assign the resolve function
        });

        // Create a timeout promise
        const timeoutPromise = new Promise((_, rejectTimeout) => {
            setTimeout(() => {
                rejectTimeout(new Error('Timeout exceeded'));
            }, maxWaitTime);
        });

        // Use Promise.race to wait for either the trigger or the timeout
        Promise.race([externalPromise, timeoutPromise]).then(resolve).catch(reject);
    });
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
    if (!window.inputMidiDevice) {
        return;
    }
    scenes_updated[scene_num] = true;
    disableWatchers = true;
    // get all parameters

    for (let output_num = 0; output_num < 8; output_num++) {
        for (let param of Object.keys(vm.scenes[scene_num].outputs[output_num])) {
            let value = vm.scenes[scene_num].outputs[output_num][param];
            sysex_string = `${scene_num}_${output_num}_${hash_djb(param)}`;
            // skip code and code_len
            if (param == "code_len") {
                continue;
            }
            for (let i = 0; i < 3; i++) {
                try {
                    console.log(`[sending_sysex] ${param} ${sysex_string}`);
                    send_sysex(sysex_string);
                    await waitForTriggerOrTimeout(200);
                    await Vue.nextTick();
                    break;
                } catch (error) {
                    console.log('Retrying the current iteration due to timeout...');
                }
            }
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
                console.log(`[recv] ${sysex}`);
                fields = sysex.split(" ");
                fields_ = sysex.split("_");
                // see if it starts with version=
                if (sysex.startsWith("LS") || sysex.startsWith("LN") || sysex.startsWith("LE")) {
                    // code starting, continuing, and ending needs to appended to codeTexts
                    // get the scene number from the third character
                    let scene_num = Number(sysex[2]);
                    let output_num = Number(sysex[3]);
                    let code = sysex.slice(4);
                    if (sysex[1] == "S") {
                        // starting a new code
                        codeTexts[scene_num][output_num] = code;
                    } else if (sysex[1] == "N") {
                        // continuing the code
                        codeTexts[scene_num][output_num] += code;
                    } else if (sysex[1] == "E") {
                        // ending the code
                        codeTexts[scene_num][output_num] += code;
                        let code_new = codeTexts[scene_num][output_num];
                        // update the code in the vm
                        vm.scenes[scene_num].outputs[output_num].code = code_new;
                        // update the code mirror if it is visible
                        if (vm.current_scene == scene_num && vm.current_output == output_num) {
                            Vue.nextTick(() => {
                                console.log(`[output_change] ${code_new}`);
                                let beautify_code = LuaFormatter.beautifyLua(code_new).trim();
                                console.log(`[output_change] ${beautify_code}`);
                                myCodeMirror.setValue(beautify_code);
                                outputCodeMirror.setValue("");

                            });
                        }
                        externalTrigger();
                    }
                } else if (fields_.length == 8) {
                    // update sparkline with each value
                    // iterate over every field
                    last_time_of_message_received = Date.now();
                    for (let i = 0; i < 8; i++) {
                        let value = parseFloat(fields_[i]);
                        // convert range 0-999 to -5 to 10
                        vm.updateSparkline(i, value / 9999.0 * 15.0 - 5);
                    }
                    // is connected
                    vm.device_connected = true
                } else if (fields.length == 4) {
                    // check if field [3] is a parameter
                    externalTrigger();
                    console.log(`[sysex_receieved] ${fields[0]} ${fields[1]} ${fields[2]} ${fields[3]}`);
                    let param_hash = Number(fields[2]);
                    if (param_hash == hash_djb("scene")) {
                        let scene_num = Number(fields[3]);
                        console.log(`[scene_change] ${scene_num}`);
                        vm.current_scene = scene_num;
                        updateLocalScene(scene_num);
                    } else {
                        let scene_num = Number(fields[0]);
                        let output_num = Number(fields[1]);
                        if (vm.scenes[scene_num] && vm.scenes[scene_num].outputs[output_num]) {
                            let output = vm.scenes[scene_num].outputs[output_num];
                            let param = Object.keys(output).find(key => hash_djb(key) == param_hash);
                            let value = Number(fields[3]);
                            if (output[param] != value) {
                                updateWithoutWatcher(scene_num, output_num, param, value);
                            }
                        }

                    }
                } else {
                    addToMidiConsole('sysex ' + sysex);
                }
            } else {
                addToMidiConsole('data ' + midiMessage.data);
            }
        };
    }
}


function setupMidi() {
    navigator.requestMIDIAccess({ sysex: true })
        .then((midiAccess) => {
            // Input setup
            const inputs = midiAccess.inputs.values();
            // current inputs
            let current_inputs = Object.keys(midiInputs);
            let new_inputs = [];
            for (let input of inputs) {
                new_inputs.push(input.name);
                if (input.name.includes("yoctocore") || input.name.includes("zeptocore") || input.name.includes("ectocore")) {
                    window.inputMidiDevice = input;
                    setupMidiInputListener();
                    console.log("input device connected");
                } else {
                    // add to midiInputDict if not already present
                    // check if input.name in midi_input_active
                    if (vm.midi_input_active[input.name] === undefined) {
                        console.log(`detecting input device: ${input.name}`);
                        midiInputs[input.name] = input;
                        vm.midi_input_active[input.name] = false;
                        vm.midi_input_last_message[input.name] = "";
                        input.onmidimessage = (midiMessage) => {
                            // Check if the message is a "Note On" message and extract the channel, note, and velocity
                            const [status, note, velocity] = midiMessage.data;
                            const messageType = status & 0xF0; // Extract the message type
                            const channel = status & 0x0F; // Extract the channel (0-15)

                            if (messageType === 0x90 && velocity > 0) { // Note On (0x90) and velocity > 0
                                console.log(`[${input.name}] note_on ch=${channel + 1}, note=${note}, vel=${velocity}`);
                            } else if (messageType === 0x80 || (messageType === 0x90 && velocity === 0)) {
                                console.log(`[${input.name}] note_off ch=${channel + 1}, note=${note}, vel=${velocity}`);
                            }

                            // convert the data to hex string
                            let hexString = "";
                            for (let i = 0; i < midiMessage.data.length; i++) {
                                hexString += midiMessage.data[i].toString(16).padStart(2, '0');
                            }
                            vm.midi_input_last_message[input.name] = hexString;
                            // pass through to yoctocore if it is setup
                            if (window.yoctocoreDevice && vm.midi_input_active[input.name]) {
                                window.yoctocoreDevice.send(midiMessage.data);
                            }
                        };
                    }

                }
            }
            // check to see if any of the current_inputs are not in new_inputs
            for (let input_num = 0; input_num < current_inputs.length; input_num++) {
                if (!new_inputs.includes(current_inputs[input_num])) {
                    console.log(`removing input device: ${current_inputs[input_num]}`);
                    delete midiInputs[current_inputs[input_num]];
                    delete vm.midi_input_active[current_inputs[input_num]];
                    delete vm.midi_input_last_message[current_inputs[input_num]];
                }
            }

            // Output setup
            const outputs = midiAccess.outputs.values();
            for (let output of outputs) {
                // console.log(`[output] ${output.name}`);
                if (output.name.includes("yoctocore")) {
                    window.yoctocoreDevice = output;
                    console.log("output device connected");
                    sysex_string = `0_0_${hash_djb("scene")}`;
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

function floatTo14Bit(floatValue) {
    if (floatValue < 0 || floatValue > 1) {
        throw new Error("Input float must be in the range 0 to 1.");
    }

    // Scale float to 14-bit range (0 to 16383)
    const scaledValue = Math.round(floatValue * 16383);

    // Extract the high 7 bits (most significant bits)
    const high7Bits = (scaledValue >> 7) & 0x7F;

    // Extract the low 7 bits (least significant bits)
    const low7Bits = scaledValue & 0x7F;

    // Return the two 7-bit numbers
    return { high7Bits, low7Bits };
}

function send_voltage(channel, scaled_voltage) {
    // Convert the scaled voltage to 14-bit
    const { high7Bits, low7Bits } = floatTo14Bit(scaled_voltage);

    // Construct the SysEx message
    if (window.yoctocoreDevice) {
        window.yoctocoreDevice.send([0xB0 + (channel - 1 + 8), high7Bits, low7Bits]);
    }
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
            console.log(`[.send] ${str}`);
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

        // // ask for sparkline data (doubles as check if connected)
        const sparkline_update_time_ms = 50;
        setTimeout(() => {
            setInterval(() => {
                if (Date.now() - last_time_of_message_received > sparkline_update_time_ms * 2) {
                    window.yoctocoreDevice && window.yoctocoreDevice.send([0x9F, 0x01, 0x01]);
                }
                // need to fix this to prevent multiple connects
                // if (Date.now() - last_time_of_message_received > sparkline_update_time_ms * 4) {
                //     vm.device_connected = false;
                //     setupMidi();
                // }
            }, sparkline_update_time_ms);
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
                    quantization: 0,
                    v_oct: 1.0,
                    root_note: 60,
                    min_voltage: -5,
                    max_voltage: 10,
                    slew_time: 0,
                    portamento: 0,
                    midi_channel: 0,
                    midi_priority_channel: 0,
                    midi_cc: 0,
                    clock_tempo: 120,
                    clock_division: 9,
                    lfo_period: 1,
                    lfo_depth: 1,
                    lfo_waveform: 0,
                    attack: 0.1,
                    decay: 0.1,
                    sustain: 0.7,
                    release: 2.1,
                    linked_to: 0,
                    probability: 100,
                    code: `function main()
	return 60
end`,
                    duration: 1,
                    setpoint_voltage: 0,
                })),
            }))
        );
        const clockDivisions = [
            "/512", "/256", "/128", "/64", "/32", "/16", "/8", "/4", "/2",
            "x1", "x2", "x3", "x4", "x6", "x8", "x12", "x16", "x24", "x48"
        ];
        const current_scene = ref(0);
        const current_output = ref(0);
        const selected_output = computed(() => {
            return scenes.value[current_scene.value].outputs[current_output.value];
        });
        const codeMirrorArea = ref(null);
        const device_connected = ref(false);
        const midi_input_active = ref({});
        const midi_input_last_message = ref({});
        const note_names = computed(() => {
            const noteNames = [
                "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
            ];
            const notes = [];
            for (let i = 0; i <= 127; i++) {
                const octave = Math.floor(i / 12) - 1;
                const note = noteNames[i % 12];
                notes.push(`${note}${octave}`);
            }
            return notes;
        });
        const sparklineData = reactive(
            Array.from({ length: 8 }, () => []) // Initialize with empty arrays for 8 sparklines
        );

        function updateSparkline(index, value) {
            const maxDataPoints = 50; // Limit the number of data points per sparkline

            // Add the new value to the data buffer
            sparklineData[index].push(value);

            // Ensure the buffer size doesn't exceed the maximum
            if (sparklineData[index].length > maxDataPoints) {
                sparklineData[index].shift();
            }

            // Redraw the sparkline
            const mode = scenes.value[current_scene.value].outputs[index].mode;
            drawSparkline(index, sparklineData[index], mode);
        }

        let code_last = "";
        async function clearLua() {
            console.log(`[clearLua]`);
            code_last = "Zzzz";
            // clear the output 
            outputCodeMirror.setValue(''); // Clear the CodeMirror output
            document.getElementById('output').textContent = ''; // Clear the text content for the buffer
            // beautify the current code
            let code = scenes.value[current_scene.value].outputs[current_output.value].code;
            let beautify_code = LuaFormatter.beautifyLua(code).trim();
            myCodeMirror.setValue(beautify_code);
            setTimeout(() => {
                myCodeMirror.refresh();
                outputCodeMirror.refresh();
            }, 50);
        }
        async function executeLua() {
            // using https://github.com/Doridian/LuaJS
            let outputElement = document.getElementById('output');
            let code = myCodeMirror.getValue();

            let new_code = code;
            let beautify_code = code;
            try {
                new_code = LuaFormatter.minifyLua(code).trim();
            } catch (error) {
                console.log(`[executeLua]: ${error}`);
                outputCodeMirror.setValue(`${error}`);
                return;
            }
            let output_num = current_output.value + 1;
            if (new_code != code_last) {
                console.log(`[executeLua]: new state`);

                luaState.then(async (L) => {
                    await L.run(`update_env(${output_num}, [[${new_code}]])`);
                });
                await luaState;
                code_last = new_code;
                outputCodeMirror.setValue(`-- ${new_code.length} bytes`);
            }
            luaState.then(async (L) => {
                let value;
                try {
                    value = await L.run(`return test_env_main(${output_num})`); //value == [3]
                } catch (error) {
                    value = error;
                }
                // wait for promise
                await value;
                // get the value
                console.log(`[executeLua]: ${value}`);
                // append to output
                outputCodeMirror.setValue(outputCodeMirror.getValue() + '\n' + value);
            });
            // console.log(`[executeLua]: ${new_code}`);
            beautify_code = LuaFormatter.beautifyLua(code).trim();
            myCodeMirror.setValue(beautify_code);
            // set the new code to the current output of the current scene
            scenes.value[current_scene.value].outputs[current_output.value].code = new_code;

            if (window.yoctocoreDevice) {
                // upload the code to the device.
                // split new_code into 32 byte chunks
                let chunk_size = 32;
                let num_chunks = Math.ceil(new_code.length / chunk_size);
                for (let i = 0; i < num_chunks; i++) {

                    let chunk = new_code.slice(i * chunk_size, (i + 1) * chunk_size);
                    if (i == 0) {
                        chunk = `LN${current_scene.value}${current_output.value}${chunk}`;
                    } else {
                        chunk = `LA${current_scene.value}${current_output.value}${chunk}`;
                    }
                    console.log(`[executeLua] ${chunk}`);
                    // send chunk and wait 10 ms
                    send_sysex(chunk);
                    await new Promise(r => setTimeout(r, 10));
                }
            }
        }

        function darkMode() {
            document.body.classList.toggle('dark-mode');
            // code mirror dark mode
            myCodeMirror.setOption("theme", document.body.classList.contains('dark-mode') ? "material-darker" : "default");
            outputCodeMirror.setOption("theme", document.body.classList.contains('dark-mode') ? "material-darker" : "default");
            localStorage.setItem('darkMode', document.body.classList.contains('dark-mode') ? 'enabled' : 'disabled');
        }
        // Function to initialize dark mode based on saved preference
        function initializeDarkMode() {
            const darkModePreference = localStorage.getItem('darkMode');

            // Check the preference and apply the dark mode if enabled
            if (darkModePreference === 'enabled') {
                document.body.classList.add('dark-mode');
                myCodeMirror.setOption("theme", "material-darker");
                outputCodeMirror.setOption("theme", "material-darker");
            } else {
                myCodeMirror.setOption("theme", "default");
                outputCodeMirror.setOption("theme", "default");
            }
        }
        function doBoardReset() {
            send_sysex("diskmode1");
        }
        function toggleActivation(inputName) {
            console.log(`Toggling activation for ${inputName}`);
            midi_input_active.value[inputName] = !midi_input_active.value[inputName];
            console.log(`Activated: ${midi_input_active.value[inputName]}`);
        }

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
                    return 'mode-clock';
                case 5:
                    return 'mode-lfo';
                case 6:
                    return 'mode-sequencer';
                case 7:
                    return 'mode-code';
                default:
                    return '';
            }
        }

        const defaultValues = {
            0: {
                quantization: 0,
            },
            1: {
                quantization: 1,
                root_note: 60,
                min_voltage: 0,
                max_voltage: 10,
                probability: 100,
            },
            2: {

            },
            3: {

            },
            4: {
                min_voltage: 0,
            },
            5: {
                min_voltage: 0,

            },
            6: {
                quantization: 0,
            },
            7: {
                code: `function main()
	return 60
end`,
            },
        };

        // watch output change
        watch(
            () => selected_output.value,
            (newOutput) => {
                console.log(`[output_change] ${current_scene.value} ${current_output.value}`);
                Vue.nextTick(() => {
                    let beautify_code = LuaFormatter.beautifyLua(newOutput.code).trim();
                    console.log(`[output_change] ${beautify_code}`);
                    myCodeMirror.setValue(beautify_code);
                    clearLua();
                });
            }
        );

        // watch scene change 
        watch(
            () => current_scene.value,
            (newScene) => {
                // send sysex to update the scene
                updateLocalScene(newScene);
                // update the scene on the device
                sysex_string = `0_0_${hash_djb("scene")}_${newScene}`;
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

        // Watcher for mode changes to update default values
        watch(
            () => selected_output.value.mode,
            (newMode) => {
                console.log(`Mode changed to ${newMode}`);
                if (newMode == 7) {
                    clearLua();
                }
                // const modeDefaults = defaultValues[newMode] || {};
                // console.log(`Setting defaults for mode ${newMode}:`, modeDefaults);
                // Object.assign(selected_output.value, modeDefaults);
            }
        );
        onMounted(() => {
            scenes.value[current_scene.value].outputs.forEach((_, index) => {
                drawSparkline(index, sparklineData[index]); // Initially empty
            });
            // setup code mirror
            myCodeMirror = CodeMirror.fromTextArea(document.getElementById('mytext'));
            myCodeMirror.setOption("lineNumbers", true);
            myCodeMirror.setSize("100%", "100%");
            outputCodeMirror = CodeMirror.fromTextArea(document.getElementById('output'));
            outputCodeMirror.setOption("lineNumbers", true);
            outputCodeMirror.setSize("100%", "100%");
            // var outputElement = document.getElementById('output');
            // emscripten.print = function (x) {
            //     // Ensure the content doesn't persist between runs
            //     outputElement.textContent = (outputElement.textContent ? outputElement.textContent + '\n' : '') + x;
            // };
            //             // TODO remove this
            //             // switch to mode 7
            //             // on next tick
            //             Vue.nextTick(() => {
            //                 scenes.value[current_scene.value].outputs[current_output.value].mode = 7;
            //                 myCodeMirror.setValue(`function main()
            //     return 60
            // end`);
            //                 setTimeout(() => {
            //                     myCodeMirror.refresh();
            //                     myCodeMirror.focus();
            //                 }, 50);
            //             });
            initializeDarkMode();


        });

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
                        // do not update code
                        if (prop == "code") {
                            return;
                        }
                        val = Number(val);
                        sysex_string = `${sceneIdx}_${outputIdx}_${hash_djb(prop)}_${val.toPrecision(4)}`;
                        console.log(`[sending_sysex] ${prop} ${sysex_string}`);
                        send_sysex(sysex_string);
                    }, 300)
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
            midi_input_active,
            midi_input_last_message,
            toggleActivation,
            doBoardReset,
            darkMode,
            note_names,
            clockDivisions,
            executeLua,
            clearLua,
            updateSparkline,
        };
    },
});

vm = app.mount('#app');

