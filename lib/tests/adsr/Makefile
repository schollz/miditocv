run: build
	./main

build:
	gcc -o main main.c -lm

check: build
	valgrind ./main 
	
clean:
	rm -f main
	rm -f *csv