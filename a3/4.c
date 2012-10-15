// chew up an unlimited amount of computer time doing essentially nothing. 
#include <stdio.h> 
main() { 
    long dummy=0; 
    // the form of this program intentionally interferes with the optimizer
    // so that it cannot be optimized, but still chews up large amounts of
    // time. 
    while (1) { 
	long i=0; 
	dummy++; 
	for (i=0; i<1000000; i++) dummy++; 
#ifdef DEBUG
	fprintf(stderr, "completed 1000000 iterations\n"); 
#endif /* DEBUG */ 
    } 
} 
