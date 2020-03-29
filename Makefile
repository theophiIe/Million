compile:
	gcc -Wall million.c -L. -liof -lm -g -o million
	
clean:
	rm million
