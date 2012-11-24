all: a5 see.R
a5: t5.o a5.o 
	gcc -ggdb t5.o a5.o -o a5
t5: t5.o a5.o 
	gcc -ggdb -o t5 t5.o a5.o
t5.o: t5.c programs.c t5.h 
	gcc -ggdb -c t5.c 
a5.o: a5.c t5.h 
	gcc -ggdb -c a5.c 

clean:
	rm a5
