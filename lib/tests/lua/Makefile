run:
	xxd -i script.lua > script.c
	gcc -o main main.c -llua5.3 -lm
	./main

clean:
	rm -rf script.c main