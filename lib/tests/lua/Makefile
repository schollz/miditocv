run:
	xxd -i script.lua > script.c
	xxd -i sequins.lua > sequins.c
	gcc -o main main.c -llua5.3 -lm
	./main
	ls -l script.c
	ls -l sequins.c

clean:
	rm -rf script.c main