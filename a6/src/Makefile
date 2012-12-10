CFLAGS = -pedantic -ggdb -Wall -Werror
CC = gcc $(CFLAGS)

a6: a6.o
	$(CC) a6.o -o a6 -lrt


a6.o: a6.c
	$(CC) -c a6.c 

clean:
	rm  a6 *.o TAGS
