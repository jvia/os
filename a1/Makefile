#CFLAGS = -Wall -Werror -std=gnu99 -D _GNU_SOURCE -ggdb
CFLAGS = -std=gnu99 -D _GNU_SOURCE -ggdb
CC = gcc $(CFLAGS)

all: mylife

mylife: mylife.o
	$(CC) mylife.o -o $@

mylife.o: mylife.c
	$(CC) -c $< -o $@

life: life.o
	$(CC) mylife.o -o $@

life.o: life.c
	$(CC) life.c -o $@

run_original: life
	./life

clean:
	rm -f *.o mylife life




