const { createApp, ref, computed, watch } = Vue;

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

        // Watcher for mode changes to update default values
        watch(
            () => selected_output.value.mode,
            (newMode) => {
                const modeDefaults = defaultValues[newMode] || {};
                Object.assign(selected_output.value, modeDefaults);
            }
        );

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

            // Create a debounced logger if it doesn't exist
            if (!debounceMap.has(key)) {
                debounceMap.set(
                    key,
                    debounce((sceneIdx, outputIdx, prop, val) => {
                        console.log(`scene[${sceneIdx}].output[${outputIdx}].${prop} = ${val}`);
                    }, 1000)
                );
            }

            // Call the debounced logger with the current change
            debounceMap.get(key)(sceneIndex, outputIndex, property, value);
        }



        // Watcher with per-output debouncing
        watch(
            () => JSON.stringify(scenes.value),
            (newVal, oldVal) => {
                const parseNew = JSON.parse(newVal);
                const parseOld = JSON.parse(oldVal);

                // Iterate through each scene and output
                parseNew.forEach((scene, sceneIndex) => {
                    scene.outputs.forEach((output, outputIndex) => {
                        const oldOutput = parseOld[sceneIndex]?.outputs[outputIndex];

                        // Check for changes in "mode"
                        if (output.mode !== oldOutput?.mode) {
                            logChange(sceneIndex, outputIndex, 'mode', output.mode);
                        }

                        // Check for changes in "min_voltage"
                        if (output.min_voltage !== oldOutput?.min_voltage) {
                            logChange(sceneIndex, outputIndex, 'min_voltage', output.min_voltage);
                        }

                        // Check for changes in "max_voltage"
                        if (output.max_voltage !== oldOutput?.max_voltage) {
                            logChange(sceneIndex, outputIndex, 'max_voltage', output.max_voltage);
                        }
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
        };
    },
});

app.mount('#app');
