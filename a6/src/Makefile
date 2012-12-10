CFLAGS = -pedantic -ggdb -Wall -Werror
CC = gcc $(CFLAGS)

run: a6 testfile.dat
	./a6 testfile.dat

a6: a6.o
	$(CC) a6.o -o a6 -lrt


a6.o: a6.c
	$(CC) -c a6.c 

testfile.dat:
	ln -s ~/comp/111/a/a6/testfile.dat .

clean:
	rm  a6 *.o TAGS
