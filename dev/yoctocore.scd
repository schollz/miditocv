(
s.waitForBoot({
	MIDIClient.init;
	MIDIClient.destinations.do{ |dest , i|
		if (dest.name.contains("yoctocore")) {
			~yoctocore_midi = MIDIOut.newByName(dest.device, dest.name);
		};
	};

	// ~yotcocore_set_volts(channel, voltage);
	// channels are 1-based (1 to 8)
	// voltage is -5.0 to +10.0
	yotcocore_set_volts = {
		arg channel,voltage;
		var sysExString = "setvolt_" ++ channel.asString ++ "_" ++ voltage.asString;
		var sysExMessage = [240] ++ sysExString.ascii ++ [247];
		if (~yoctocore_midi.notNil) {
			~yoctocore_midi.sysex(Int8Array.newFrom(sysExMessage));
		};
	};

	// ~yotcocore_set_note(channel, voltage);
	// channels are 1-based (1 to 8)
	// voltage is -5.0 to +10.0
	~yoctocore_set_note = {
		arg channel, note, velocity;
		MIDIClient.destinations.do { |dest, i|
			if (~yoctocore_midi.notNil){
				~yoctocore_midi.noteOn(channel, note, velocity);
			};
		};
	};
});
)

// set channel, volts
~yotcocore_set_volts.(1,-10.0);
~yoctocore_set_note.(1,60,120);