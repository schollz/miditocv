#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "../../../lua-5.4.6/src/lauxlib.h"
#include "../../../lua-5.4.6/src/lua.h"
#include "../../../lua-5.4.6/src/lualib.h"

#define __linux__ 1

#include "../../luavm.h"

// Test helper functions
static int test_count = 0;
static int test_passed = 0;
static int test_failed = 0;

#define ASSERT(condition, message) do { \
    test_count++; \
    if (condition) { \
        printf("   ✓ %s\n", message); \
        test_passed++; \
    } else { \
        printf("   ✗ FAILED: %s\n", message); \
        test_failed++; \
    } \
} while(0)

#define ASSERT_FLOAT_EQ(a, b, tolerance, message) do { \
    test_count++; \
    if (fabs((a) - (b)) < tolerance) { \
        printf("   ✓ %s (%.6f)\n", message, (float)(a)); \
        test_passed++; \
    } else { \
        printf("   ✗ FAILED: %s (expected %.6f, got %.6f)\n", message, (float)(b), (float)(a)); \
        test_failed++; \
    } \
} while(0)

void test_on_beat() {
    printf("\n=== Test: on_beat Function ===\n");

    const char *code =
        "bpm = 120\n"
        "volts = 0\n"
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 5.0\n"
        "    trigger = true\n"
        "  else\n"
        "    volts = 0.0\n"
        "    trigger = false\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(1, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test with on=true
    result = luaRunOnBeat(1, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT_FLOAT_EQ(volts, 5.0, 0.001, "volts set to 5.0 when on=true");
    ASSERT(trigger == true, "trigger set to true when on=true");

    // Test with on=false
    result = luaRunOnBeat(1, false, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(false) executed without error");
    ASSERT_FLOAT_EQ(volts, 0.0, 0.001, "volts set to 0.0 when on=false");
    ASSERT(trigger == false, "trigger set to false when on=false");
}

void test_on_knob() {
    printf("\n=== Test: on_knob Function ===\n");

    const char *code =
        "function on_knob(value)\n"
        "  volts = value * 10.0\n"
        "  trigger = value > 0.5\n"
        "end\n";

    int result = luaUpdateEnvironment(2, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test with value=0.5
    result = luaRunOnKnob(2, 0.5, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_knob(0.5) executed without error");
    ASSERT_FLOAT_EQ(volts, 5.0, 0.001, "volts calculated correctly (0.5 * 10.0 = 5.0)");
    ASSERT(trigger == false, "trigger is false when value <= 0.5");

    // Test with value=0.8
    result = luaRunOnKnob(2, 0.8, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_knob(0.8) executed without error");
    ASSERT_FLOAT_EQ(volts, 8.0, 0.001, "volts calculated correctly (0.8 * 10.0 = 8.0)");
    ASSERT(trigger == true, "trigger is true when value > 0.5");
}

void test_on_button() {
    printf("\n=== Test: on_button Function ===\n");

    const char *code =
        "button_state = false\n"
        "function on_button(pressed)\n"
        "  button_state = pressed\n"
        "  if pressed then\n"
        "    volts = 3.3\n"
        "    trigger = true\n"
        "  else\n"
        "    volts = 0.0\n"
        "    trigger = false\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(3, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test button press
    result = luaRunOnButton(3, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_button(true) executed without error");
    ASSERT_FLOAT_EQ(volts, 3.3, 0.001, "volts set to 3.3 when button pressed");
    ASSERT(trigger == true, "trigger set when button pressed");

    // Test button release
    result = luaRunOnButton(3, false, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_button(false) executed without error");
    ASSERT_FLOAT_EQ(volts, 0.0, 0.001, "volts set to 0.0 when button released");
    ASSERT(trigger == false, "trigger cleared when button released");
}

void test_on_cc() {
    printf("\n=== Test: on_cc Function ===\n");

    const char *code =
        "function on_cc(cc, value)\n"
        "  volts = value / 127.0 * 5.0\n"
        "  trigger = (cc == 7)\n"
        "end\n";

    int result = luaUpdateEnvironment(4, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test CC 7 with value 127
    result = luaRunOnCc(4, 7, 127, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_cc(7, 127) executed without error");
    ASSERT_FLOAT_EQ(volts, 5.0, 0.001, "volts calculated correctly (127/127 * 5.0 = 5.0)");
    ASSERT(trigger == true, "trigger set when CC is 7");

    // Test CC 1 with value 64
    result = luaRunOnCc(4, 1, 64, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_cc(1, 64) executed without error");
    ASSERT_FLOAT_EQ(volts, 2.519685, 0.001, "volts calculated correctly (64/127 * 5.0)");
    ASSERT(trigger == false, "trigger cleared when CC is not 7");
}

void test_on_note_on() {
    printf("\n=== Test: on_note_on Function ===\n");

    const char *code =
        "function on_note_on(channel, note, velocity)\n"
        "  -- Convert MIDI note to voltage (1V/octave, C4 = 60 = 0V)\n"
        "  volts = (note - 60) / 12.0\n"
        "  trigger = (velocity > 0)\n"
        "end\n";

    int result = luaUpdateEnvironment(5, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test middle C (note 60)
    result = luaRunOnNoteOn(5, 1, 60, 100, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_note_on(1, 60, 100) executed without error");
    ASSERT_FLOAT_EQ(volts, 0.0, 0.001, "volts = 0.0 for middle C (note 60)");
    ASSERT(trigger == true, "trigger set when velocity > 0");

    // Test C5 (note 72)
    result = luaRunOnNoteOn(5, 1, 72, 127, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_note_on(1, 72, 127) executed without error");
    ASSERT_FLOAT_EQ(volts, 1.0, 0.001, "volts = 1.0 for C5 (one octave up)");
    ASSERT(trigger == true, "trigger set when velocity > 0");

    // Test C3 (note 48)
    result = luaRunOnNoteOn(5, 1, 48, 64, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_note_on(1, 48, 64) executed without error");
    ASSERT_FLOAT_EQ(volts, -1.0, 0.001, "volts = -1.0 for C3 (one octave down)");
    ASSERT(trigger == true, "trigger set when velocity > 0");
}

void test_on_note_off() {
    printf("\n=== Test: on_note_off Function ===\n");

    const char *code =
        "active_notes = {}\n"
        "function on_note_on(channel, note, velocity)\n"
        "  active_notes[note] = true\n"
        "  volts = (note - 60) / 12.0\n"
        "  trigger = true\n"
        "end\n"
        "function on_note_off(channel, note)\n"
        "  active_notes[note] = nil\n"
        "  trigger = false\n"
        "  volts = 0.0\n"
        "end\n";

    int result = luaUpdateEnvironment(6, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Trigger a note on
    result = luaRunOnNoteOn(6, 1, 60, 100, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_note_on executed without error");
    ASSERT(trigger == true, "trigger set on note on");

    // Trigger note off
    result = luaRunOnNoteOff(6, 1, 60, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_note_off(1, 60) executed without error");
    ASSERT_FLOAT_EQ(volts, 0.0, 0.001, "volts cleared on note off");
    ASSERT(trigger == false, "trigger cleared on note off");
}

void test_multiple_environments() {
    printf("\n=== Test: Multiple Environments Isolation ===\n");

    // Set up two different environments
    const char *code1 =
        "counter = 0\n"
        "function on_beat(on)\n"
        "  if on then counter = counter + 1 end\n"
        "  volts = counter\n"
        "end\n";

    const char *code2 =
        "counter = 100\n"
        "function on_beat(on)\n"
        "  if on then counter = counter + 10 end\n"
        "  volts = counter\n"
        "end\n";

    int result1 = luaUpdateEnvironment(7, code1);
    int result2 = luaUpdateEnvironment(8, code2);
    ASSERT(result1 == 0, "Environment 7 code loaded");
    ASSERT(result2 == 0, "Environment 8 code loaded");

    float volts1, volts2;
    bool volts_new, trigger;
    float gate;

    // Run on_beat in environment 7
    luaRunOnBeat(7, true, &volts1, &volts_new, &trigger, &gate);
    ASSERT_FLOAT_EQ(volts1, 1.0, 0.001, "Environment 7 counter incremented to 1");

    // Run on_beat in environment 8
    luaRunOnBeat(8, true, &volts2, &volts_new, &trigger, &gate);
    ASSERT_FLOAT_EQ(volts2, 110.0, 0.001, "Environment 8 counter incremented to 110");

    // Run again in environment 7 to verify isolation
    luaRunOnBeat(7, true, &volts1, &volts_new, &trigger, &gate);
    ASSERT_FLOAT_EQ(volts1, 2.0, 0.001, "Environment 7 counter still independent (2)");

    // Verify environment 8 unchanged
    luaRunOnBeat(8, false, &volts2, &volts_new, &trigger, &gate);
    ASSERT_FLOAT_EQ(volts2, 110.0, 0.001, "Environment 8 counter unchanged (110)");
}

void test_bpm_setting() {
    printf("\n=== Test: BPM Setting ===\n");

    const char *code =
        "bpm = 140\n"
        "function on_beat(on)\n"
        "  volts = bpm / 120.0\n"
        "end\n";

    int result = luaUpdateEnvironment(1, code);
    ASSERT(result == 0, "Code loaded successfully");

    float bpm = luaGetBPM(1);
    ASSERT_FLOAT_EQ(bpm, 140.0, 0.001, "BPM retrieved correctly (140)");

    // Update BPM via code
    const char *code2 =
        "bpm = 180\n"
        "function on_beat(on)\n"
        "  volts = bpm / 120.0\n"
        "end\n";

    result = luaUpdateEnvironment(1, code2);
    ASSERT(result == 0, "Code updated successfully");

    bpm = luaGetBPM(1);
    ASSERT_FLOAT_EQ(bpm, 180.0, 0.001, "BPM updated correctly (180)");
}

void test_error_handling() {
    printf("\n=== Test: Error Handling ===\n");

    // Test syntax error during load
    const char *bad_syntax =
        "function on_beat(on\n"  // Missing closing parenthesis
        "  volts = 5.0\n"
        "end\n";

    int result = luaUpdateEnvironment(1, bad_syntax);
    ASSERT(result != 0, "Syntax error detected during load");

    // Test runtime error
    const char *runtime_error =
        "function on_beat(on)\n"
        "  volts = undefined_variable + 5\n"
        "end\n";

    result = luaUpdateEnvironment(1, runtime_error);
    ASSERT(result == 0, "Code with runtime error loaded");

    float volts;
    bool volts_new, trigger;
    float gate;
    result = luaRunOnBeat(1, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result != 0, "Runtime error detected during execution");
}

int main() {
    printf("=== Basic Lua VM Functionality Tests ===\n");

    // Initialize Lua
    int init_result = luaInit();
    if (init_result != 0) {
        printf("✗ Failed to initialize Lua VM\n");
        return 1;
    }
    printf("✓ Lua VM initialized successfully\n");

    // Run all tests
    test_on_beat();
    test_on_knob();
    test_on_button();
    test_on_cc();
    test_on_note_on();
    test_on_note_off();
    test_multiple_environments();
    test_bpm_setting();
    test_error_handling();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);

    if (test_failed == 0) {
        printf("\n✓ All tests passed!\n");
        return 0;
    } else {
        printf("\n✗ Some tests failed\n");
        return 1;
    }
}
