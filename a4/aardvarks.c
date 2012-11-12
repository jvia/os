#include "anthills.h" 
#include <pthread.h>
#include <semaphore.h>

#define TRUE  1
#define FALSE 0

int initialized = FALSE;
sem_t hill[ANTHILLS];
int ants_left[ANTHILLS] = {
  ANTS_PER_HILL, ANTS_PER_HILL, ANTS_PER_HILL, ANTS_PER_HILL
};

void eat(char name, int i)
{
  int semval, full_slots;

  // Calc how many aardvarks on the hillx
  sem_getvalue(&hill[i], &semval);
  full_slots = AARDVARKS_PER_HILL - semval;

  if ((ants_left[i] > full_slots)  &&  (sem_trywait(&hill[i])) != -1) {
    if (ants_left[i] > 0) {
      slurp(name, i);
      --ants_left[i];
    }
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
    for (i = 0; i < ANTHILLS; ++i)
      sem_init(&hill[i], 0, AARDVARKS_PER_HILL);
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

