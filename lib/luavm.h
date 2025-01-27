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
  lua_getglobal(L, "update_env");
  if (!lua_isfunction(L, -1)) {
    printf(
        "[luaUpdateEnvironment] update_env not defined or not a function.\n");
    lua_pop(L, 1);
    return 1;
  }

  lua_pushinteger(L, index);
  lua_pushstring(L, code);

  if (lua_pcall(L, 2, 0, 0) != LUA_OK) {
    printf("[luaUpdateEnvironment] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  // Remove error message
    return 1;
  }
  // No need to pop anything here
  return 0;
}

int luaInit() {
  if (L != NULL) {
    return 0;
  }
  L = luaL_newstate();  // Create a new Lua state
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
    lua_pushinteger(L, n+1);
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
  if (lua_pcall(L, 1, 3, 0) != LUA_OK) {
    printf("[luaGetVoltsAndTrigger] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);  // Pop error message
    return false;
  }

  *volts = lua_tonumber(L, -3);
  *volt_set = lua_toboolean(L, -2);
  *trigger = lua_toboolean(L, -1);
  lua_pop(L, 3);  // Pop v, new_volts, and do_trigger

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

bool luaRunOnBeat(int index, bool on, float *volts, bool *volts_new,
                  bool *trigger) {
  if (!withLuaEnv(index)) return false;

  lua_getfield(L, -1, "on_beat");  // Push envs[index].on_beat onto the stack
  if (!lua_isfunction(L, -1)) {    // Check if on_beat is a function
    lua_pop(L, 3);                 // Pop envs, envs[index], and on_beat
    return false;
  }

  lua_pushboolean(L, on);  // Push the argument for on_beat
  if (lua_pcall(L, 1, LUA_MULTRET, 0) !=
      LUA_OK) {  // Call on_beat with 1 argument, expecting 1 return
    printf("[luaRunOnBeat] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 4);  // Pop envs, envs[index], on_beat, and error message
    return false;
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    if (lua_isstring(L, -1)) {
      const char *result = lua_tostring(L, -1);
      // printf("Result: %s\n", result);
    }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsAndTrigger(index, volts, volts_new, trigger);
  return true;
}

bool luaRunOnKnob(int index, float val, float *volts,
                  bool *volts_new, bool *trigger) {
  if (!withLuaEnv(index)) return false;

  lua_getfield(L, -1, "on_knob");  // Push envs[index].on_knob onto the stack
  if (!lua_isfunction(L, -1)) {    // Check if on_knob is a function
    lua_pop(L, 3);                 // Pop envs, envs[index], and on_knob
    return false;
  }

  // Push the arguments
  lua_pushnumber(L, val);
  if (lua_pcall(L, 1, LUA_MULTRET, 0) !=
      LUA_OK) {  // Call on_beat with 2 argument, expecting 1 return
    printf("[luaRunOnKnob] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 4);  // Pop envs, envs[index], on_beat, and error message
    return false;
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result = lua_tostring(L, -1);
    //   printf("Result: %s\n", result);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsAndTrigger(index, volts, volts_new, trigger);
  return true;
}

bool luaRunOnButton(int index, bool val, float *volts,
                    bool *volts_new, bool *trigger) {
  if (!withLuaEnv(index)) return false;

  lua_getfield(L, -1,
               "on_button");     // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_button is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return false;
  }

  // Push the arguments
  lua_pushboolean(L, val);
  if (lua_pcall(L, 1, LUA_MULTRET, 0) !=
      LUA_OK) {  // Call on_beat with 2 argument, expecting 1 return
    printf("[luaRunOnButton] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 4);  // Pop envs, envs[index], on_beat, and error message
    return false;
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result = lua_tostring(L, -1);
    //   printf("Result: %s\n", result);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]

  luaGetVoltsAndTrigger(index, volts, volts_new, trigger);
  return true;
}

bool luaRunOnNoteOn(int index, int channel, int note, int velocity,
                    float *volts, bool *volts_new, bool *trigger) {
  if (!withLuaEnv(index)) return false;

  lua_getfield(L, -1,
               "on_note_on");    // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_note_on is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return false;
  }

  // Push the arguments
  lua_pushinteger(L, channel);
  lua_pushinteger(L, note);
  lua_pushinteger(L, velocity);
  if (lua_pcall(L, 3, LUA_MULTRET, 0) !=
      LUA_OK) {  // Call on_note_on with 2 argument, expecting 1 return
    printf("[luaRunOnNoteOn] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 4);  // Pop envs, envs[index], on_beat, and error message
    return false;
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result = lua_tostring(L, -1);
    //   printf("Result: %s\n", result);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsAndTrigger(index, volts, volts_new, trigger);
  return true;
}

bool luaRunOnNoteOff(int index, int channel, int note, float *volts,
                     bool *volts_new, bool *trigger) {
  if (!withLuaEnv(index)) return false;

  lua_getfield(L, -1,
               "on_note_off");   // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_note_off is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return false;
  }

  // Push the arguments
  lua_pushinteger(L, channel);
  lua_pushinteger(L, note);
  if (lua_pcall(L, 2, LUA_MULTRET, 0) !=
      LUA_OK) {  // Call on_note_off with 2 argument, expecting 1 return
    printf("[luaRunOnNoteOff] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 4);  // Pop envs, envs[index], on_beat, and error message
    return false;
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result = lua_tostring(L, -1);
    //   printf("Result: %s\n", result);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]
  luaGetVoltsAndTrigger(index, volts, volts_new, trigger);
  return true;
}

bool luaRunOnCc(int index, int cc, int value, float *volts, bool *volts_new,
                bool *trigger) {
  if (!withLuaEnv(index)) return false;

  lua_getfield(L, -1, "on_cc");  // Push envs[index].on_button onto the stack
  if (!lua_isfunction(L, -1)) {  // Check if on_cc is a function
    lua_pop(L, 3);               // Pop envs, envs[index], and on_button
    return false;
  }

  // Push the arguments
  lua_pushinteger(L, cc);
  lua_pushinteger(L, value);
  if (lua_pcall(L, 2, LUA_MULTRET, 0) !=
      LUA_OK) {  // Call on_cc with 2 argument, expecting 1 return
    printf("[luaRunOnCc] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 4);  // Pop envs, envs[index], on_beat, and error message
    return false;
  }

  // Check how many results were returned
  int num_results = lua_gettop(L) - 2;
  // Process the results if needed
  if (num_results > 0) {
    // uncomment to print result
    // if (lua_isstring(L, -1)) {
    //   const char *result = lua_tostring(L, -1);
    //   printf("Result: %s\n", result);
    // }
    // Pop all results to clean up the stack
    lua_pop(L, num_results);
  }

  lua_pop(L, 2);  // Pop envs[index]

  luaGetVoltsAndTrigger(index, volts, volts_new, trigger);
  return true;
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
      if (luaRunOnBeat(channel, beat % 2 == 0, &volts, &volts_set, &trigger)) {
        printf("luaRunOnBeat out[%d].volts: %f, %d, %d\n", channel, volts,
               volts_set, trigger);
      }
      if (luaGetVoltsAndTrigger(channel, &volts, &volts_set, &trigger)) {
        printf("out[%d].volts: %f, %d, %d\n", channel, volts, volts_set,
               trigger);
      }
    }
  }

  // garbage collect
  luaGarbageCollect();
  lua_close(L);

  return 0;
}
