// blow the stack with an infinite and intentionally antisocial recursion. 
#include <stdio.h> 

void oink() { 
    int garbage[1024]; // push this onto the stack. 
#ifdef DEBUG
    fprintf(stderr, "allocated %d bytes of stack memory\n", 1024*sizeof(int)); 
#endif /* DEBUG */ 
    oink(); 
} 

main() { 
    oink(); 
} 
