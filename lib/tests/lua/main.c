#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For sleep

#include "script.c"

int main() {
  lua_State *L = luaL_newstate();  // Create a new Lua state
  luaL_openlibs(L);                // Open standard libraries

  // Load Lua script from embedded string
  if (luaL_loadbuffer(L, (const char *)script_lua, script_lua_len,
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

  // Define the new Lua function addone()
  const char *addone_code =
      "function addone()\n"
      "  local num = random_number()\n"
      "  return num + 0.1\n"
      "end";

  // Execute the new Lua function definition
  if (luaL_dostring(L, addone_code) != LUA_OK) {
    printf("Error defining addone function: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }

  // Set up random number generator seed
  luaL_dostring(L, "math.randomseed(os.time())");

  // Call the Lua function and get the result
  for (int i = 0; i < 5; i++) {
    lua_getglobal(L, "addone");             // Get the function from Lua
    if (lua_pcall(L, 0, 1, 0) != LUA_OK) {  // Call the function
      printf("Error: %s\n", lua_tostring(L, -1));
      lua_close(L);
      return 1;
    }

    double val = lua_tonumber(L, -1);  // Get the return value as a double
    lua_pop(L, 1);                     // Pop the result from the stack
    printf("random float: %.2f\n", val);

    sleep(1);  // Sleep for 1 second
  }

  lua_close(L);  // Clean up the Lua state
  return 0;
}