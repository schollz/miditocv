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

lua_State *L;

/**
function update_env(i, code)
    envs[i] = new_env(code)
end
**/
int luaUpdateEnvironment(int index, const char *code) {
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
  for (int i = 0; i < 8; i++) {
    luaUpdateEnvironment(i, "");
  }

  return 0;
}

float luaGetBPM(int index) {
  lua_getglobal(L, "envs");
  lua_pushinteger(L, index);
  lua_gettable(L, -2);         // envs[index]
  lua_getfield(L, -1, "bpm");  // envs[index].bpm
  if (!lua_isnumber(L, -1)) {
    printf("[luaGetBPM] envs[%d].bpm not defined or not a number.\n", index);
    lua_pop(L, 3);  // Remove envs, envs[index], and error message
    return 0.0f;
  }
  float bpm = lua_tonumber(L, -1);
  lua_pop(L, 2);  // Remove envs[index] and bpm
  return bpm;
}

float luaGetVolts(int index) {
  lua_getglobal(L, "envs");
  lua_pushinteger(L, index);
  lua_gettable(L, -2);           // envs[index]
  lua_getfield(L, -1, "volts");  // envs[index].volts
  if (!lua_isnumber(L, -1)) {
    printf("[luaGetVolts] envs[%d].volts not defined or not a number.\n",
           index);
    lua_pop(L, 3);  // Remove envs, envs[index], and error message
    return 0.0f;
  }
  float volts = lua_tonumber(L, -1);
  lua_pop(L, 2);  // Remove envs[index] and volts
  return volts;
}

bool luaGetTrigger(int index) {
  lua_getglobal(L, "envs");
  lua_pushinteger(L, index);
  lua_gettable(L, -2);             // envs[index]
  lua_getfield(L, -1, "trigger");  // envs[index].trigger
  bool trigger = lua_toboolean(L, -1);
  lua_pop(L, 2);  // Remove envs[index] and trigger
  return trigger;
}

bool luaRunOnBeat(int index, int beat) {
  lua_getglobal(L, "envs");
  lua_pushinteger(L, index);
  lua_gettable(L, -2);             // envs[index]
  lua_getfield(L, -1, "on_beat");  // envs[index].on_beat
  if (!lua_isfunction(L, -1)) {
    // printf("[luaRunOnBeat] envs[%d].on_beat not defined or not a
    //        function.\n ",index);
    lua_pop(L, 3);  // Remove envs, envs[index], and error message
    return false;
  }
  lua_pushinteger(L, beat);
  if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
    printf("[luaRunOnBeat] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 3);  // Remove envs, envs[index], and error message
    return false;
  }
  // return the result as a string
  if (!lua_isstring(L, -1)) {
    // printf("[luaRunOnBeat] envs[%d].on_beat did not return a string.\n",
    // index);
    lua_pop(L, 3);  // Remove envs, envs[index], and error message
    return false;
  }
  const char *result = lua_tostring(L, -1);
  lua_pop(L, 2);  // Remove envs[index] and result
  // print string
  // printf("env[%d].on_beat(%d): %s\n", index, beat, result);
  return true;
}

int luaTest() {
  luaInit();
  const char *code0 =
      "a=S{'a4','b4','c4'}\n"
      "bpm = 120\n"
      "volts = 5\n"
      "function on_beat(beat)\n"
      "  b = a()\n"
      " volts = to_cv(b)\n"
      " trigger = beat % 2 == 0\n"
      "return b\n"
      "end\n";
  luaUpdateEnvironment(0, code0);
  const char *code1 = "volts=10;";
  luaUpdateEnvironment(1, code1);
  // get bpm from env 0
  printf("bpm0: %f\n", luaGetBPM(0));
  printf("bpm1: %f\n", luaGetBPM(1));
  for (uint8_t beat = 0; beat < 16; beat++) {
    for (uint8_t channel = 0; channel < 2; channel++) {
      if (luaRunOnBeat(channel, beat)) {
        printf("volts0: %f, %d\n", luaGetVolts(channel),
               luaGetTrigger(channel));
      }
    }
  }

  return 0;
}