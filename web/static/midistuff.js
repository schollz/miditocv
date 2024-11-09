function addToMidiConsole(message) {
    console.log('MIDI message:', message);
}
function setupMidiInputListener() {
    if (window.inputMidiDevice) {
        window.inputMidiDevice.onmidimessage = (midiMessage) => {
            // check if sysex
            if (midiMessage.data[0] == 0xf0) {
                // convert the sysex to string 
                var sysex = "";
                for (var i = 1; i < midiMessage.data.length - 1; i++) {
                    sysex += String.fromCharCode(midiMessage.data[i]);
                }
                // see if it starts with version=
                if (sysex.startsWith("version=")) {
                    app.deviceVersion = sysex.split("=")[1];
                    console.log(`[setupMidiInputListener] Device version: ${app.deviceVersion}`);
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
    console.log("Setting up MIDI");
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
                    break;
                }
            }


        })
        .catch((error) => {
            console.error("Could not access MIDI devices.", error);
        });
}

document.addEventListener('DOMContentLoaded', () => {
    // if chrome and on desktop
    if (window.chrome && window.chrome.app) {
        // setupMidi();
    }
});