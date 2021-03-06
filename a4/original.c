#include "anthills.h" 
#include <pthread.h>
#include <semaphore.h>

#define TRUE  1
#define FALSE 0

int initialized=FALSE; // semaphores and mutexes are not initialized 

// define your mutexes and semaphores here 
void *my_thread(void *input) { 
  char aname = *(char *)input; // name of aardvark, for debugging
  while (chow_time()) { 
    // Should only eat if there enough ants and not too many aardvarks
    slurp(aname, lrand48() % ANTHILLS);
  }
  return NULL; 
} 

// first thread initializes mutexes 
void *thread_A(void *input) { 
  if (!initialized) {
    initialized=TRUE;
  }  
  return my_thread(input); 
}

// other threads proceed after initialization
void *thread_B(void *input) { 
  while (!initialized);
  return my_thread(input); 
} 
void *thread_C(void *input) { 
  while (!initialized)
    return my_thread(input); 
} 
void *thread_D(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_E(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_F(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_G(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_H(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_I(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_J(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 
void *thread_K(void *input) { 
    while (!initialized);
    return my_thread(input); 
} 

