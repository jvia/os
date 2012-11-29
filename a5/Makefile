CFLAGS = -pedantic -ggdb
CC = gcc $(CFLAGS)

all: see.R t5
	$(CC) t5.o a5.o -o a5

t5: t5.o a5.o 
	$(CC) -o t5 t5.o a5.o

t5.o: t5.c programs.c t5.h 
	gcc -c t5.c 

a5.o: a5.c t5.h 
	$(CC) -c a5.c 

tags:
	etags `find . -name "*.c" -o -name "*.cpp" -o -name "*.h"`

clean-tags:
	rm TAGS

docs: clean-docs
	doxygen doc.conf

clean-docs:
	rm -r docs

clean:
	rm  a5 *.o
