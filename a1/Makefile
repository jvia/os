#CFLAGS = -Wall -Werror -std=gnu99 -D _GNU_SOURCE -ggdb
#CFLAGS = -std=gnu99 -D _GNU_SOURCE -ggdb
CC = gcc $(CFLAGS)

all: mylife life

mylife: mylife.o
	$(CC) mylife.o -o $@
mylife.o: mylife.c
	$(CC) -c $< -o $@


life: life.o
	$(CC) mylife.o -o $@
life.o: life.c
	$(CC) life.c -o $@

cheat: cheat.o
	$(CC) cheat.o -o $@
cheat.o: cheat.c
	$(CC) cheat.c -o $@

clean:
	rm -f *.o mylife life




