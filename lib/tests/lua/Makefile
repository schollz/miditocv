run:
	luamin -f ../../../web/static/globals.lua > globals.lua || true
	mkdir -p lib
	xxd -i globals.lua > lib/lua_globals.h
	mv lib/lua_globals.h ../../lua_globals.h
	rm -rf globals.lua lib
	gcc -o main main.c -llua5.3 -lm
	./main
	# valgrind ./main

clean:
	rm -rf script.c main

test:
	lua ../../../web/static/globals.lua