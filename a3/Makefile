CFLAGS =  -std=gnu99 -D _GNU_SOURCE -pthread
C_FILES = $(wildcard *.c)
APPS = $(patsubst %.c,%,$(C_FILES))

all: $(APPS)

test: all
	./watch ./1 \\
	./watch ./2 \\
	./watch ./3 \\
	./watch ./4 \\
	./watch ./5

clean:
	rm -f $(APPS)
