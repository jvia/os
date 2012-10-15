// try to take over all memory and allocate it on your heap
#include <stdio.h> 
#include <malloc.h> 

main() 
{ 
    while(1) { 
	int *p=(int *)malloc(1024*sizeof(int)); 
#ifdef DEBUG
	fprintf(stderr, "allocated %d bytes\n", 1024*sizeof(int)); 
#endif /* DEBUG */ 
    } 
} 
