#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For sleep

int main() {
  lua_State *L = luaL_newstate();  // Create a new Lua state
  luaL_openlibs(L);                // Open standard libraries

  // Define the Lua script directly as a string
  const char *lua_code =
      "function random_number()\n"
      "  return math.random(1, 10) + 100.123\n"
      "end";

  // Run the Lua script to define the function
  if (luaL_dostring(L, lua_code) != LUA_OK) {
    printf("Error: %s\n", lua_tostring(L, -1));
    lua_close(L);
    return 1;
  }

  // Set up random number generator seed
  luaL_dostring(L, "math.randomseed(os.time())");

  // Call the Lua function and get the result
  for (int i = 0; i < 5; i++) {
    lua_getglobal(L, "random_number");      // Get the function from Lua
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
