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

void test_gate_default() {
    printf("\n=== Test: Gate Default Value (should be 0) ===\n");

    const char *code =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 5.0\n"
        "    trigger = true\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(1, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test that gate defaults to 0
    result = luaRunOnBeat(1, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT_FLOAT_EQ(gate, 0.0, 0.001, "gate defaults to 0.0");
    ASSERT(trigger == true, "trigger is true");
}

void test_gate_zero() {
    printf("\n=== Test: Gate = 0 (immediate trigger) ===\n");

    const char *code =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 3.3\n"
        "    trigger = true\n"
        "    gate = 0\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(2, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test with gate = 0
    result = luaRunOnBeat(2, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT_FLOAT_EQ(gate, 0.0, 0.001, "gate set to 0.0");
    ASSERT(trigger == true, "trigger is true");
    ASSERT_FLOAT_EQ(volts, 3.3, 0.001, "volts set to 3.3");
}

void test_gate_half() {
    printf("\n=== Test: Gate = 0.5 (half beat duration) ===\n");

    const char *code =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 2.5\n"
        "    trigger = true\n"
        "    gate = 0.5\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(3, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test with gate = 0.5
    result = luaRunOnBeat(3, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT_FLOAT_EQ(gate, 0.5, 0.001, "gate set to 0.5");
    ASSERT(trigger == true, "trigger is true");
    ASSERT_FLOAT_EQ(volts, 2.5, 0.001, "volts set to 2.5");
}

void test_gate_full() {
    printf("\n=== Test: Gate = 1.0 (full beat duration) ===\n");

    const char *code =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 4.0\n"
        "    trigger = true\n"
        "    gate = 1.0\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(4, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test with gate = 1.0
    result = luaRunOnBeat(4, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT_FLOAT_EQ(gate, 1.0, 0.001, "gate set to 1.0");
    ASSERT(trigger == true, "trigger is true");
    ASSERT_FLOAT_EQ(volts, 4.0, 0.001, "volts set to 4.0");
}

void test_gate_per_environment() {
    printf("\n=== Test: Gate per Environment Isolation ===\n");

    const char *code1 =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 1.0\n"
        "    trigger = true\n"
        "    gate = 0.25\n"
        "  end\n"
        "end\n";

    const char *code2 =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 2.0\n"
        "    trigger = true\n"
        "    gate = 0.75\n"
        "  end\n"
        "end\n";

    int result1 = luaUpdateEnvironment(5, code1);
    int result2 = luaUpdateEnvironment(6, code2);
    ASSERT(result1 == 0, "Environment 5 code loaded");
    ASSERT(result2 == 0, "Environment 6 code loaded");

    float volts1, volts2;
    bool volts_new, trigger;
    float gate1, gate2;

    // Run in environment 5
    luaRunOnBeat(5, true, &volts1, &volts_new, &trigger, &gate1);
    ASSERT_FLOAT_EQ(gate1, 0.25, 0.001, "Environment 5 gate is 0.25");

    // Run in environment 6
    luaRunOnBeat(6, true, &volts2, &volts_new, &trigger, &gate2);
    ASSERT_FLOAT_EQ(gate2, 0.75, 0.001, "Environment 6 gate is 0.75");

    // Verify environment 5 gate unchanged
    luaRunOnBeat(5, true, &volts1, &volts_new, &trigger, &gate1);
    ASSERT_FLOAT_EQ(gate1, 0.25, 0.001, "Environment 5 gate still 0.25");
}

void test_gate_with_out_table() {
    printf("\n=== Test: Gate with out[] table ===\n");

    const char *code =
        "function on_beat(on)\n"
        "  if on then\n"
        "    volts = 3.0\n"
        "    trigger = true\n"
        "    gate = 0.3\n"
        "    out[2].volts = 5.0\n"
        "    out[2].trigger = true\n"
        "    out[2].gate = 0.7\n"
        "  end\n"
        "end\n";

    int result = luaUpdateEnvironment(7, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // Test with gate set via out[] table (should override environment gate)
    result = luaRunOnBeat(7, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    // Note: The gate returned should be for environment 7 (0.3)
    // The out[2].gate would need to be retrieved separately for output 2
    ASSERT_FLOAT_EQ(gate, 0.3, 0.001, "gate set to 0.3 for environment");
}

void test_trigger_only_on_low_to_high() {
    printf("\n=== Test: Trigger Only on Low to High ===\n");

    const char *code =
        "last_trigger = false\n"
        "function on_beat(on)\n"
        "  -- Only trigger on transition from false to true\n"
        "  if on and not last_trigger then\n"
        "    trigger = true\n"
        "    gate = 0.5\n"
        "  else\n"
        "    trigger = false\n"
        "  end\n"
        "  last_trigger = on\n"
        "  volts = on and 5.0 or 0.0\n"
        "end\n";

    int result = luaUpdateEnvironment(8, code);
    ASSERT(result == 0, "Code loaded successfully");

    float volts;
    bool volts_new;
    bool trigger;
    float gate;

    // First call: on=false, no trigger
    result = luaRunOnBeat(8, false, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(false) executed without error");
    ASSERT(trigger == false, "trigger is false when on=false");

    // Second call: on=true, should trigger (low to high)
    result = luaRunOnBeat(8, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT(trigger == true, "trigger is true on low to high transition");
    ASSERT_FLOAT_EQ(gate, 0.5, 0.001, "gate set to 0.5");

    // Third call: on=true, no trigger (already high)
    result = luaRunOnBeat(8, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT(trigger == false, "trigger is false when already high");

    // Fourth call: on=false, no trigger
    result = luaRunOnBeat(8, false, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(false) executed without error");
    ASSERT(trigger == false, "trigger is false when going low");

    // Fifth call: on=true, should trigger again (low to high)
    result = luaRunOnBeat(8, true, &volts, &volts_new, &trigger, &gate);
    ASSERT(result == 0, "on_beat(true) executed without error");
    ASSERT(trigger == true, "trigger is true on second low to high transition");
}

int main() {
    printf("=== Gate Parameter Functionality Tests ===\n");

    // Initialize Lua
    int init_result = luaInit();
    if (init_result != 0) {
        printf("✗ Failed to initialize Lua VM\n");
        return 1;
    }
    printf("✓ Lua VM initialized successfully\n");

    // Run all tests
    test_gate_default();
    test_gate_zero();
    test_gate_half();
    test_gate_full();
    test_gate_per_environment();
    test_gate_with_out_table();
    test_trigger_only_on_low_to_high();

    // Print summary
    printf("\n=== Test Summary ===\n");
    printf("Total tests: %d\n", test_count);
    printf("Passed: %d\n", test_passed);
    printf("Failed: %d\n", test_failed);

    if (test_failed == 0) {
        printf("\n✓ All gate tests passed!\n");
        return 0;
    } else {
        printf("\n✗ Some gate tests failed\n");
        return 1;
    }
}
