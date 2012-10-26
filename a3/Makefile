CFLAGS =  -std=gnu99 -D _GNU_SOURCE -pthread
C_FILES = $(wildcard *.c)
APPS = $(patsubst %.c,%,$(C_FILES))

all: $(APPS)
clean:
	rm -f $(APPS)

# all: 1 2 3 4 5 watch
# 1: 1.c
# 2: 2.c
# 3: 3.c 
# 4: 4.c
# 5: 5.c
# watch: watch.c
