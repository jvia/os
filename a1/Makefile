#CFLAGS = -Wall -Werror -std=gnu99 -D _GNU_SOURCE -ggdb
#CFLAGS = -std=gnu99 -D _GNU_SOURCE -ggdb
CC = gcc $(CFLAGS)

all: 
	$(CC) -g cheat.c      -o cheat
	$(CC) -g cheat-ia64.c -o cheat64
	$(CC) -g life.c       -o life
	$(CC) -g mylife.c     -o mylife
	$(CC) -g mylife64.c   -o mylife64
	$(CC) -g sig_life.c   -o sig_life

clean:
	rm -f *.o cheat cheat64 life mylife mylife64 sig_life




