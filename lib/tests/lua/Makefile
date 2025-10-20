LUA_DIR = ../../../lua-5.4.6
LUA_INC = $(LUA_DIR)/src
LUA_LIB = $(LUA_DIR)/src/liblua.a

run:
	luamin -f ../../../web/static/globals.lua > globals.lua || true
	mkdir -p lib
	xxd -i globals.lua > lib/lua_globals.h
	mv lib/lua_globals.h ../../lua_globals.h
	rm -rf globals.lua lib
	gcc -o main main.c -llua5.3 -lm
	./main
	# valgrind ./main

test_panic_simple: test_panic_simple.c ../../luavm.h ../../lua_globals.h
	gcc -o test_panic_simple test_panic_simple.c -I$(LUA_INC) $(LUA_LIB) -lm -ldl
	./test_panic_simple

clean:
	rm -rf script.c main test_panic_simple

test: test_panic_simple