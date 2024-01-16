all:
	make tone-interpreter

tone-interpreter:
	gcc tone-interpreter.c -Wall -o tone-interpreter -lpulse -lpulse-simple -lm

clean:
	rm tone-interpreter

remake:
	rm tone-interpreter
	make tone-interpreter
