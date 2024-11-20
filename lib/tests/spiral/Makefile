run:
	gcc -o main main.c -lm
	./main > data.txt
	gnuplot -p -e "plot 'data.txt' using 1:2 with lines title 'X-Y Plot'"