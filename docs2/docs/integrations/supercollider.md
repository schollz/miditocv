---
sidebar_position: 1
---

# SuperCollider

SuperCollider can be integrated easily with the yoctocore. Here is an example of how to use the yoctocore with SuperCollider.

```c title="yoctocore.scd"
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
  ~yotcocore_set_volts = {
    arg channel,voltage;
    var sysExString = "setvolt_" ++ channel.asString ++ "_" ++ voltage.asString;
    var sysExMessage = [240] ++ sysExString.ascii ++ [247];
    if (~yoctocore_midi.notNil) {
      ["yoctocore_set_volts",channel,voltage].postln;
      ~yoctocore_midi.sysex(Int8Array.newFrom(sysExMessage));
    };
  };

  // ~yotcocore_set_note_on(channel, voltage);
  // channels are 1-based (1 to 8)
  ~yoctocore_set_note_on = {
    arg channel, note, velocity;
    if (~yoctocore_midi.notNil){
      ["yoctocore_note_on",channel,note,velocity].postln;
      ~yoctocore_midi.noteOn(channel-1, note, velocity);
    };
  };

  // ~yotcocore_set_note_off(channel, voltage);
  // channels are 1-based (1 to 8)
  ~yoctocore_set_note_off = {
    arg channel, note;
    if (~yoctocore_midi.notNil){
      ["yoctocore_note_on",channel,note].postln;
      ~yoctocore_midi.noteOff(channel-1, note);
    };
  };
});
)

// set channel, volts
~yotcocore_set_volts.(1,-10.0);
// note on and note offs
~yoctocore_set_note_on.(1,60,30);
~yoctocore_set_note_off.(1,60,30);

```
