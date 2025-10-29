// needed for lua
#include <errno.h>
#include <stdbool.h>
#include <sys/types.h>
#include <unistd.h>

int _link(const char *oldpath, const char *newpath) {
  errno = ENOSYS;
  return -1;
}

int _unlink(const char *pathname) {
  errno = ENOSYS;
  return -1;
}

//
#ifndef __linux__
#include <lauxlib.h>
#include <lua.h>
#include <lualib.h>
#endif
//
#include "lua_globals.h"

lua_State *L = NULL;
static bool lua_global_panic_flag = false;

// Forward declarations
void luaClearPanicFlag();

// Custom panic handler that prevents device from becoming unresponsive
static int custom_panic_handler(lua_State *L) {
  const char *msg = lua_tostring(L, -1);
  if (msg == NULL) msg = "error object is not a string";
  printf("PANIC: unprotected error in call to Lua API (%s)\n", msg);
  printf("Setting global panic flag to prevent device unresponsiveness\n");
  
  // Set global panic flag instead of aborting
  lua_global_panic_flag = true;
  
  // Note: After a panic, the Lua state may be in an inconsistent state.
  // The panic flag will prevent further Lua operations until code is reloaded.
  // When code is reloaded, luaClearPanicFlag() is called to reset the state.
  
  // Return to Lua - this prevents abort() from being called
  // The error will propagate but won't crash the system
  return 0;
}

/**
function update_env(i, code)
    envs[i] = new_env(code)
end
**/
int luaUpdateEnvironment(int index, const char *code) {
  printf("[luaUpdateEnvironment] index: %d, code: %s\n", index, code);
  if (L == NULL) {
    printf("[luaUpdateEnvironment] Lua VM not initialized.\n");
    return 1;
  }
  
  // Clear panic flag when updating environment
  luaClearPanicFlag();
  
  lua_getglobal(L, "update_env");
  if (!lua_isfunction(L, -1)) {
    printf(
        "[luaUpdateEnvironment] update_env not defined or not a function.\n");
    lua_pop(L, 1);
    return 1;
  }

  lua_pushinteger(L, index);
  lua_pushstring(L, code);

  if (lua_pcall(L, 2, 1, 0) != LUA_OK) {
    printf("[luaUpdateEnvironment] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  // Remove error message
    return 1;
  }
  
  // Check if update_env returned false (code load failed)
  bool success = lua_toboolean(L, -1);
  lua_pop(L, 1);  // Remove return value
  
  if (!success) {
    printf("[luaUpdateEnvironment] Code failed to load\n");
    return 1;
  }
  
  return 0;
}

int luaInit() {
  if (L != NULL) {
    return 0;
  }
  L = luaL_newstate();  // Create a new Lua state
  
  // Install custom panic handler to prevent device from becoming unresponsive
  lua_atpanic(L, custom_panic_handler);
  
  luaL_openlibs(L);     // Open standard libraries

  // Load Lua script from embedded string
  if (luaL_loadbuffer(L, (const char *)globals_lua, globals_lua_len,
                      "embedded_script") != LUA_OK) {
    printf("Error loading Lua script: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }

  // Execute the loaded Lua script
  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
    printf("Error executing Lua script: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }

  // create 8 environments
  for (int i = 1; i <= 8; i++) {
    luaUpdateEnvironment(i, "");
  }

  return 0;
}

bool Lua_eval_simple(const char *script, size_t script_len,
                     const char *chunkname) {
  if (luaL_loadbuffer(L, script, script_len, chunkname) != LUA_OK) {
    printf("Error loading Lua repl eval: %s\n", lua_tostring(L, -1));
    return false;
  }

  if (lua_pcall(L, 0, 0, 0) != LUA_OK) {
    printf("Error executing Lua repl eval: %s\n", lua_tostring(L, -1));
    return false;
  }

  return true;
}

bool withLuaEnv(int index) {
  lua_getglobal(L, "envs");   // Push envs onto the stack
  if (!lua_istable(L, -1)) {  // Check if envs is a table
    lua_pop(L, 1);            // Pop envs
    return false;
  }

  lua_pushinteger(L, index);
  lua_gettable(L, -2);        // Push envs[index] onto the stack
  if (!lua_istable(L, -1)) {  // Check if envs[index] is a table
    lua_pop(L, 2);            // Pop envs and envs[index]
    return false;
  }

  return true;
}

void luaSetShift(bool shift) {
  lua_pushboolean(L, shift);
  lua_setglobal(L, "shift");
}

void luaSetButton(int n, bool val) {
  lua_getglobal(L, "button");
  if (lua_istable(L, -1)) {
    lua_pushinteger(L, n + 1);
    lua_pushboolean(L, val);
    lua_settable(L, -3);
  } else {
    printf("Error: 'button' is not a table\n");
  }
  lua_pop(L, 1);
}

bool luaGetVoltsAndTrigger(int index, float *volts, bool *volt_set,
                           bool *trigger) {
  lua_getglobal(L, "volts_and_trigger");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 1);  // Pop volts_and_trigger
    return false;
  }

  lua_pushinteger(L, index);
  if (lua_pcall(L, 1, 4, 0) != LUA_OK) {
    printf("[luaGetVoltsAndTrigger] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  // Pop error message
    return false;
  }

  *volts = lua_tonumber(L, -4);
  *volt_set = lua_toboolean(L, -3);
  *trigger = lua_toboolean(L, -2);
  // gate is at -1 but we're not using it in this function
  lua_pop(L, 4);  // Pop v, new_volts, do_trigger, and gate

  return true;
}

bool luaGetVoltsTriggerAndGate(int index, float *volts, bool *volt_set,
                                bool *trigger, float *gate) {
  lua_getglobal(L, "volts_and_trigger");
  if (!lua_isfunction(L, -1)) {
    lua_pop(L, 1);  // Pop volts_and_trigger
    return false;
  }

  lua_pushinteger(L, index);
  if (lua_pcall(L, 1, 4, 0) != LUA_OK) {
    printf("[luaGetVoltsTriggerAndGate] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  // Pop error message
    return false;
  }

  *volts = lua_tonumber(L, -4);
  *volt_set = lua_toboolean(L, -3);
  *trigger = lua_toboolean(L, -2);
  *gate = lua_tonumber(L, -1);
  lua_pop(L, 4);  // Pop v, new_volts, do_trigger, and gate

  return true;
}

float luaGetBPM(int index) {
  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1, "bpm");  // Push envs[index].bpm onto the stack
  if (!lua_isnumber(L, -1)) {  // Check if bpm is a number
    lua_pop(L, 3);             // Pop envs, envs[index], and bpm
    return -1;
  }

  float bpm = lua_tonumber(L, -1);
  lua_pop(L, 3);  // Pop bpm, envs[index], and envs
  return bpm;
}

// Check if a Lua panic has occurred
bool luaHasPanicked() {
  return lua_global_panic_flag;
}

// Clear the global panic flag (called when reloading code)
void luaClearPanicFlag() {
  lua_global_panic_flag = false;
}

int luaRunOnBeat(int index, bool on, float *volts, bool *volts_new,
                  bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }
  
  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1, "on_beat");  // Push envs[index].on_beat onto the stack
  if (!lua_isfunction(L, -1)) {    // Check if on_beat is a function
    lua_pop(L, 3);                 // Pop envs, envs[index], and on_beat
    return -1;
  }

  lua_pushboolean(L, on);  // Push the argument for on_beat
  int result = lua_pcall(L, 1, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_beat with 1 argument, expecting 1 return
    printf("[luaRunOnBeat] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code (non-zero indicates panic/error)
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    if (lua_isstring(L, -1)) {
      const char *result_str = lua_tostring(L, -1);
      // printf("Result: %s\n", result_str);
    }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnKnob(int index, float val, float *volts, bool *volts_new,
                  bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }
  
  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1, "on_knob");  // Push envs[index].on_knob onto the stack
  if (!lua_isfunction(L, -1)) {    // Check if on_knob is a function
    lua_pop(L, 3);                 // Pop envs, envs[index], and on_knob
    return -1;
  }

  // Push the arguments
  lua_pushnumber(L, val);
  int result = lua_pcall(L, 1, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_beat with 2 argument, expecting 1 return
    printf("[luaRunOnKnob] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnButton(int index, bool val, float *volts, bool *volts_new,
                    bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }
  
  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1,
               "on_button");     // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_button is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return -1;
  }

  // Push the arguments
  lua_pushboolean(L, val);
  int result = lua_pcall(L, 1, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_beat with 2 argument, expecting 1 return
    printf("[luaRunOnButton] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]

  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnNoteOn(int index, int channel, int note, int velocity,
                    float *volts, bool *volts_new, bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }
  
  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1,
               "on_note_on");    // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_note_on is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return -1;
  }

  // Push the arguments
  lua_pushinteger(L, channel);
  lua_pushinteger(L, note);
  lua_pushinteger(L, velocity);
  int result = lua_pcall(L, 3, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_note_on with 2 argument, expecting 1 return
    printf("[luaRunOnNoteOn] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnNoteOff(int index, int channel, int note, float *volts,
                     bool *volts_new, bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }
  
  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1,
               "on_note_off");   // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_note_off is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return -1;
  }

  // Push the arguments
  lua_pushinteger(L, channel);
  lua_pushinteger(L, note);
  int result = lua_pcall(L, 2, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_note_off with 2 argument, expecting 1 return
    printf("[luaRunOnNoteOff] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnCc(int index, int cc, int value, float *volts, bool *volts_new,
                bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }

  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1, "on_cc");  // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_cc is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return -1;
  }

  // Push the arguments
  lua_pushinteger(L, cc);
  lua_pushinteger(L, value);
  int result = lua_pcall(L, 2, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_cc with 2 argument, expecting 1 return
    printf("[luaRunOnCc] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]

  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnKeyPressure(int index, int channel, int note, int pressure,
                        float *volts, bool *volts_new, bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }

  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1, "on_key_pressure");  // Push envs[index].on_key_pressure onto the stack
  if (!lua_isfunction(L, -1)) {            // Check if on_key_pressure is a function
    lua_pop(L, 3);                         // Pop envs, envs[index], and on_key_pressure
    return -1;
  }

  // Push the arguments
  lua_pushinteger(L, channel);
  lua_pushinteger(L, note);
  lua_pushinteger(L, pressure);
  int result = lua_pcall(L, 3, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_key_pressure with 3 arguments
    printf("[luaRunOnKeyPressure] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]

  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

int luaRunOnChannelPressure(int index, int channel, int pressure,
                            float *volts, bool *volts_new, bool *trigger, float *gate) {
  // Check for global panic flag
  if (lua_global_panic_flag) {
    return LUA_ERRRUN; // Return error to indicate panic state
  }

  if (!withLuaEnv(index)) return -1;

  lua_getfield(L, -1, "on_channel_pressure");  // Push envs[index].on_channel_pressure onto the stack
  if (!lua_isfunction(L, -1)) {                // Check if on_channel_pressure is a function
    lua_pop(L, 3);                             // Pop envs, envs[index], and on_channel_pressure
    return -1;
  }

  // Push the arguments
  lua_pushinteger(L, channel);
  lua_pushinteger(L, pressure);
  int result = lua_pcall(L, 2, LUA_MULTRET, 0);
  if (result != LUA_OK) {  // Call on_channel_pressure with 2 arguments
    printf("[luaRunOnChannelPressure] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Pop error message, envs[index], and envs (pcall consumed function+args)
    return result;  // Return error code
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result_str = lua_tostring(L, -1);
    //   printf("Result: %s\n", result_str);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]

  luaGetVoltsTriggerAndGate(index, volts, volts_new, trigger, gate);
  return 0;  // Success
}

void luaGarbageCollect() { lua_gc(L, LUA_GCCOLLECT, 0); }

int luaTest() {
  luaInit();
  const char *code0 =
      "a=S{'a4','b4','c4'}\n"
      "bpm = 120\n"
      "volts = 5\n"
      "function on_beat(on)\n"
      "  b = a()\n"
      " volts = to_cv(b)\n"
      "if volts > 0.5 then \n"
      " out[2].volts = 5.2\n end \n"
      " trigger = on\n"
      "return b\n"
      "end\n";
  luaUpdateEnvironment(1, code0);
  const char *code1 = "volts=10;";
  luaUpdateEnvironment(2, code1);
  // get bpm from env 0
  printf("bpm0: %f\n", luaGetBPM(0));
  printf("bpm1: %f\n", luaGetBPM(1));
  for (uint8_t beat = 0; beat < 16; beat++) {
    for (uint8_t channel = 1; channel <= 2; channel++) {
      float volts;
      bool volts_set;
      bool trigger;
      float gate;
      if (luaRunOnBeat(channel, beat % 2 == 0, &volts, &volts_set, &trigger, &gate)) {
        printf("luaRunOnBeat out[%d].volts: %f, %d, %d, gate: %f\n", channel, volts,
               volts_set, trigger, gate);
      }
      if (luaGetVoltsTriggerAndGate(channel, &volts, &volts_set, &trigger, &gate)) {
        printf("out[%d].volts: %f, %d, %d, gate: %f\n", channel, volts, volts_set,
               trigger, gate);
      }
    }
  }

  // garbage collect
  luaGarbageCollect();
  lua_close(L);

  return 0;
}
