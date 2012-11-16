#include "/comp/111/a/a4/anthills.h"
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

#define TRUE  1
#define FALSE 0


struct aardvark {
  int hill;     // Hill being slurped
  double time; //  Time when slurping finishes
};


int initialized = FALSE;
sem_t hill[ANTHILLS];
int slurping[ANTHILLS];
int ants_left[ANTHILLS];
struct aardvark aardvarks[AARDVARKS];
pthread_t unlocker;


/**
 * Manages the eating in a thread safe way.
 *
 * Manages the number of aardvarks on a hill and the number of ants on
 * a hill for optimality.
 */
void eat(char name, int i)
{
  int avark = name - 'A';
  if ((ants_left[i] > slurping[i])  &&  (sem_trywait(&hill[i])) != -1) {
    ++slurping[i];
    aardvarks[avark].hill = i;
    aardvarks[avark].time = elapsed() + 1.;
    slurp(name, i);
    --ants_left[i];
    --slurping[i];
    sem_post(&hill[i]);
  }
}

/**
 * Thread code. Simply calls eat while there are ants left.
 */
void *my_thread(void *input) { 
  char aname = *(char*) input;
  while (chow_time())
    eat(aname, lrand48() % ANTHILLS);
  return NULL; 
} 

/**
 * Initializes state and then runs thread code.
 */
void *thread_A(void *input) { 
  if (!initialized) {
    int i;

    for(i = 0; i < ANTHILLS; ++i) {
      ants_left[i] = ANTS_PER_HILL;
      slurping[i] = 0;
      sem_init(&hill[i], 0, AARDVARKS_PER_HILL);
    }

    for (i = 0; i < AARDVARKS; ++i) {
      aardvarks[i].hill = -1;
      aardvarks[i].time = 0.;
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

