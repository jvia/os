#include "anthills.h" 
#include <pthread.h>
#include <semaphore.h>

#define TRUE  1
#define FALSE 0

int initialized = FALSE;
sem_t hill[ANTHILLS];
int slurping[ANTHILLS];// = {0, 0, 0, 0};
int ants_left[ANTHILLS];// = {
//  ANTS_PER_HILL, ANTS_PER_HILL, ANTS_PER_HILL, ANTS_PER_HILL
//};


void eat(char name, int i)
{
  if ((ants_left[i] > slurping[i])  &&  (sem_trywait(&hill[i])) != -1) {
    ++slurping[i];
    slurp(name, i);
    --ants_left[i];
    --slurping[i];
    sem_post(&hill[i]);
  }
}

/**
 *
 */
void *my_thread(void *input) { 
  char aname = *(char*) input;
  while (chow_time())
    eat(aname, lrand48() % ANTHILLS);
  return NULL; 
} 

/**
 *
 */
void *thread_A(void *input) { 
  if (!initialized) {
    int i = 0;
    for (i = 0; i < ANTHILLS; ++i) {
      ants_left[i] = ANTS_PER_HILL;
      slurping[i] = 0;
      sem_init(&hill[i], 0, AARDVARKS_PER_HILL);
    }
    initialized = TRUE;
  }  
  return my_thread(input); 
}

//////////////////////////////////////////////////////////////////////
// Other threads
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

