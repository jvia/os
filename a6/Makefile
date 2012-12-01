CFLAGS = -pedantic -ggdb -Wall -Werror
CC = gcc $(CFLAGS)

all: a6 docs tags


a6:
	make -C src a6

tags: 
	cd src && etags `find . -name "*.c" -o -name "*.h"`

clean-tags:
	rm src/TAGS

docs:
	doxygen doc.conf

clean-docs:
	rm -r docs

clean: clean-docs
	make -C src clean
