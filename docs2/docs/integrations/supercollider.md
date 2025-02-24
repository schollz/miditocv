---
sidebar_position: 1
---

# SuperCollider

SuperCollider can be integrated easily with the miditocv. Here is an example of how to use the miditocv with SuperCollider.

```c title="miditocv.scd"
(
s.waitForBoot({
  MIDIClient.init;
  MIDIClient.destinations.do{ |dest , i|
    if (dest.name.contains("miditocv")) {
      ~miditocv_midi = MIDIOut.newByName(dest.device, dest.name);
    };
  };

  // ~yotcocore_set_volts(channel, voltage);
  // channels are 1-based (1 to 8)
  // voltage is -5.0 to +10.0
  ~yotcocore_set_volts = {
    arg channel,voltage;
    var sysExString = "setvolt_" ++ channel.asString ++ "_" ++ voltage.asString;
    var sysExMessage = [240] ++ sysExString.ascii ++ [247];
    if (~miditocv_midi.notNil) {
      ["miditocv_set_volts",channel,voltage].postln;
      ~miditocv_midi.sysex(Int8Array.newFrom(sysExMessage));
    };
  };

  // ~yotcocore_set_note_on(channel, voltage);
  // channels are 1-based (1 to 8)
  ~miditocv_set_note_on = {
    arg channel, note, velocity;
    if (~miditocv_midi.notNil){
      ["miditocv_note_on",channel,note,velocity].postln;
      ~miditocv_midi.noteOn(channel-1, note, velocity);
    };
  };

  // ~yotcocore_set_note_off(channel, voltage);
  // channels are 1-based (1 to 8)
  ~miditocv_set_note_off = {
    arg channel, note;
    if (~miditocv_midi.notNil){
      ["miditocv_note_on",channel,note].postln;
      ~miditocv_midi.noteOff(channel-1, note);
    };
  };
});
)

// set channel, volts
~yotcocore_set_volts.(1,-10.0);
// note on and note offs
~miditocv_set_note_on.(1,60,30);
~miditocv_set_note_off.(1,60,30);

```
