anthills: aardvarks.c original.c anthills.c anthills.h
	gcc -g -o anthills aardvarks.c anthills.c -lpthread -lrt
	gcc -g -o original original.c  anthills.c -lpthread -lrt 

clean:
	rm -f anthills original
