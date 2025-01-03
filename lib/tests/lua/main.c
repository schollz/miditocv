#include <lua5.3/lauxlib.h>
#include <lua5.3/lua.h>
#include <lua5.3/lualib.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // For sleep

#define __linux__ 1

#include "../../luavm.h"

int main() {
  luaTest();
  // print memory usage

  return 0;
}