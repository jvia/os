// print an unlimited number of lines on stdout
#include <stdio.h> 
main() { 
    int count = 0; 
    while (1) { 
	printf("This is line %d\n", ++count); 
    } 
} 
