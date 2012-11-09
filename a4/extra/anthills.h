#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <pthread.h> 
#include <sys/time.h> 

// student's solution
extern void *thread_A(void *); 
extern void *thread_B(void *); 
extern void *thread_C(void *); 
extern void *thread_D(void *); 
extern void *thread_E(void *); 
extern void *thread_F(void *); 
extern void *thread_G(void *); 
extern void *thread_H(void *); 
extern void *thread_I(void *); 
extern void *thread_J(void *); 
extern void *thread_K(void *); 

#define TRUE	 		1
#define FALSE	 		0	 
#define AARDVARKS		11	// number of total threads
#define ANTHILLS 		2	// number of anthills 
#define ANTS_PER_HILL 		20	// number of ants available at each hill
#define AARDVARKS_PER_HILL	3	// number of aardvarks who can share 
#define SLURP_TIME 		1
#define SWALLOW_TIME 		1
#define ERROR_TIME 		4
#define JAIL_TIME 		30 
extern int slurp(char aname, int anthill); // eat one ant. 
extern int chow_time(); // whether there are ants to eat
extern double elapsed(); // how much time has been spent? 
