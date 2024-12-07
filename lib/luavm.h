// needed for lua
#include <errno.h>
#include <sys/types.h>

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

  // create 8 environments with a main() function that returns -10
  for (int i = 0; i < 8; i++) {
    luaUpdateEnvironment(i, "function main() return -10,0 end");
  }

  return 0;
}

int luaRunMain(int index, int beat, float *voltage, int *gate) {
  lua_getglobal(L, "env_main");
  if (!lua_isfunction(L, -1)) {
    printf("[luaRunMain] env_main not defined or not a function.\n");
    lua_pop(L, 1);
    return 0;
  }

  lua_pushinteger(L, index);
  lua_pushinteger(L, beat);

  if (lua_pcall(L, 2, 2, 0) != LUA_OK) {
    printf("[luaRunMain] error: %s\n", lua_tostring(L, -1));
    lua_pop(L, 1);
    return 0;
  }

  if (lua_gettop(L) < 2) {
    printf("[luaRunMain] Not enough return values from env_main.\n");
    lua_settop(L, 0);  // Clear the stack
    return 0;
  }

  *gate = lua_tonumber(L, -1);
  lua_pop(L, 1);
  *voltage = lua_tonumber(L, -1);
  lua_pop(L, 1);

  return 1;
}

int luaTest() {
  luaInit();

  /*
  a = S{60,62,S{70,75},67}
  b = S{1,1,1,0}
  c = S{10,13,15,S{17,20}}
  function main(beat)
      a:select(beat)
      b:select(beat)
      c:select(beat)
      local u = a() + c()
      -- gate(b()>0)
      if (b()>0) then
          trig()
      end
      -- trig()
      if u~='skip' then
          do return u end
      end
  end
  */
  const char *code0 =
      "a = S{60,62,S{70,75},67}\n"
      "b = S{1,1,1,0}\n"
      "c = S{10,13,15,S{17,20}}\n"
      "function main(beat)\n"
      "    a:select(beat)\n"
      "    b:select(beat)\n"
      "    c:select(beat)\n"
      "    local u = a() + c()\n"
      "    if (b()>0) then\n"
      "        trig()\n"
      "    end\n"
      "    if u~='skip' then\n"
      "        return u\n"
      "    end\n"
      "end";
  luaUpdateEnvironment(0, code0);
  for (uint8_t i = 0; i < 8; i++) {
    float v;
    int gate;
    luaRunMain(0, i, &v, &gate);
    printf("Result: %f %d\n", v, gate);
  }

  return 0;
}