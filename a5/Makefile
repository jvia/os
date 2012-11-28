all: a5 see.R
	gcc -pedantic-std=gnu99  -ggdb t5.o a5.o -o a5
a5: t5.o a5.o 
t5: t5.o a5.o 
	gcc -pedantic -ggdb -o t5 t5.o a5.o
t5.o: t5.c programs.c t5.h 
	gcc -pedantic -ggdb -c t5.c 
a5.o: a5.c t5.h 
	gcc -pedantic -ggdb -c a5.c 

tags:
	etags `find . -name "*.c" -o -name "*.cpp" -o -name "*.h"`

clean-tags:
	rm TAGS

clean:
	rm -r a5 *.o html latex *.csv
