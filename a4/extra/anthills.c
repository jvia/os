#include "anthills.h"

// note: need to serialize concept of state
// for ease of debugging 

///////////////////////////////////////
// private data used internally
///////////////////////////////////////

// the aardvarks 
static void *(*thread_routines[AARDVARKS])(void *) = {
    thread_A, thread_B, thread_C, thread_D, 
    thread_E, thread_F, thread_G, thread_H, 
    thread_I, thread_J, thread_K
};
#define ANAME(i) ('A'+i) 
static pthread_t threads[AARDVARKS];

// state variables 
static pthread_mutex_t state_lock;	// gate the hill state machine 
static int ants_left[ANTHILLS] = { 
    ANTS_PER_HILL,ANTS_PER_HILL
}; 
static int slurping[ANTHILLS]= { 0,0 };// how many aardvarks are slurping
static int busy[AARDVARKS] = { 0,0,0,0,0,0,0,0,0,0,0 }; // is an aardvark busy? 
static int jailed[AARDVARKS] = { 0,0,0,0,0,0,0,0,0,0,0 }; // is an aardvark jailed? 
int sulking = 0; 			 // number of aardvarks sulking 
int swallowing = 0; 			 // number of aardvarks swallowing

// whether to enter debugging mode 
static int debug = FALSE; 
static int quiet = FALSE; 
static int trace = FALSE; 
static FILE *csv = NULL; 

static struct timeval start, now; 

///////////////////////////////////////
// public routines usable by student's program
///////////////////////////////////////

double elapsed() { // how much real time has elapsed?
   gettimeofday(&now,NULL); 
   time_t seconds = now.tv_sec-start.tv_sec; 
   suseconds_t microseconds = now.tv_usec-start.tv_usec; 
   double e = (double)(seconds) + ((double)microseconds)/1000000.0; 
   return e; 
} 
int chow_time() { // is there something to eat left? 
    pthread_mutex_lock(&state_lock); 
    int out = ants_left[0]>0 || ants_left[1]>0 
           || ants_left[2]>0 || ants_left[3]>0; 
    pthread_mutex_unlock(&state_lock); 
    return out; 
} 

///////////////////////////////////////
// utilities safely update state variables 
///////////////////////////////////////
static void trace_state() { 
    if (trace) 
    printf(
	"%09.6f (trace) Ants: %d %d Slurping: %d %d, Swallowing: %d, Sulking: %d\n",
	elapsed(), 
	ants_left[0], ants_left[1],  
	slurping[0],  slurping[1],  
	swallowing, sulking); 
} 
static void update_sulking(int increment) { 
    pthread_mutex_lock(&state_lock); 
    sulking+=increment; 
    trace_state(); 
    pthread_mutex_unlock(&state_lock); 
} 
static void update_swallowing(int increment) { 
    pthread_mutex_lock(&state_lock); 
    swallowing+=increment; 
    trace_state(); 
    pthread_mutex_unlock(&state_lock); 
} 
static void update_slurping(int hill, int increment) { 
    pthread_mutex_lock(&state_lock); 
    slurping[hill]+=increment; 
    trace_state(); 
    pthread_mutex_unlock(&state_lock); 
} 
static void update_ants(int hill, int increment) { 
    pthread_mutex_lock(&state_lock); 
    ants_left[hill]+=increment; 
    trace_state(); 
    pthread_mutex_unlock(&state_lock); 
} 

///////////////////////////////////////
// control aardvark concurrency
// aname must be a valid aardvark between 'A' and 'H'. 
///////////////////////////////////////
static int make_busy(char aname ) { 
    int index = aname - 'A'; 
    if (index<0 || index>=AARDVARKS) { 
	return FALSE; 
    } 
    pthread_mutex_lock(&state_lock); 
    int ret; 
    if (busy[index]) { 
       ret = FALSE; 
    } else { 
       busy[index]=TRUE; 
       ret = TRUE; 
    } 
    pthread_mutex_unlock(&state_lock); 
    return ret; 
} 
static int make_idle(char aname) { 
    int index = aname - 'A'; 
    if (index<0 || index>=AARDVARKS) { 
	return FALSE; 
    } 
    pthread_mutex_lock(&state_lock); 
    int ret; 
    if (!busy[index]) { 
       ret = FALSE; 
    } else { 
       busy[index]=FALSE; 
       ret=TRUE; 
    } 
    pthread_mutex_unlock(&state_lock); 
    return ret; 
} 
///////////////////////////////////////
// punish a naughty aardvark by making it sleep for a penalty 
///////////////////////////////////////
void make_jailed(char aname) { 
    int index = aname - 'A'; 
    if (index<0 || index>=AARDVARKS) return; 
    pthread_mutex_lock(&state_lock); 
    jailed[index]=TRUE; 
    pthread_mutex_unlock(&state_lock); 
} 
int is_jailed(char aname) { 
    int index = aname - 'A'; 
    if (index<0 || index>=AARDVARKS) FALSE; 
    int ret; 
    pthread_mutex_lock(&state_lock); 
    ret = jailed[index]; 
    pthread_mutex_unlock(&state_lock); 
    return ret; 
} 
void make_unjailed(char aname) { 
    int index = aname - 'A'; 
    if (index<0 || index>=AARDVARKS) return; 
    pthread_mutex_lock(&state_lock); 
    jailed[index]=TRUE; 
    pthread_mutex_unlock(&state_lock); 
} 

///////////////////////////////////////
// utilities carry out various actions of an aardvark 
///////////////////////////////////////
static void sulk(char aname) { // wait after an error 
    if (!quiet)
	printf("%09.6f Aardvark %c sulking\n", elapsed(), aname); 
    if (csv) 
	fprintf(csv, "%09.6f,%c,sulking\n", elapsed(), aname);
    update_sulking(1); 
    sleep(ERROR_TIME); // simulate external time passing
    update_sulking(-1); 
    if (!quiet)
	printf("%09.6f Aardvark %c idle\n", elapsed(), aname); 
    if (csv) 
	fprintf(csv, "%09.6f,%c,idle\n", elapsed(), aname);
} 
static int lick(int anthill) { // get an ant if one is there 
    int gotit; 
    pthread_mutex_lock(&state_lock);
    if (ants_left[anthill]>0) { 
	gotit=TRUE; 
	--ants_left[anthill]; 
    } else { 	
	gotit=FALSE; 
    } 
    trace_state(); 
    pthread_mutex_unlock(&state_lock);
    return gotit; 
} 
static void swallow(char aname) { // swallow an ant
    if (!quiet) 
	printf("%09.6f Aardvark %c swallowing\n", elapsed(), aname); 
    if (csv) 
	fprintf(csv, 
	    "%09.6f,%c,swallowing\n", elapsed(), aname); 
    update_swallowing(1); 
    sleep(SWALLOW_TIME); 
    update_swallowing(-1); 
    if (debug) 
	printf( 
	    "%09.6f (debug) Aardvark %c finished swallowing\n",
	    elapsed(), aname); 
    if (!quiet) printf("%09.6f Aardvark %c idle\n", elapsed(), aname); 
    if (csv) fprintf(csv, "%09.6f,%c,idle\n", elapsed(), aname); 
} 

// int ants(int anthill) {  // are there ants in an anthill? 
//     pthread_mutex_lock(&state_lock); 
//     int out = (ants_left[anthill]>0) ; 
//     pthread_mutex_unlock(&state_lock); 
//     return out;
// } 
int slurp(char aname, int anthill) { // try to slurp up an ant. 
    if (aname<'A' || aname>'A'+AARDVARKS-1) { 
	if (debug) 
	    printf(
	    "%09.6f (debug) ERROR: illegal aardvark %c specified\n",
	    elapsed(), aname); 
	sulk(aname); 
        return FALSE; 
    } else if (anthill<0 || anthill>=ANTHILLS) { 
	if (debug) 
	    printf(
	    "%09.6f (debug) ERROR: aardvark %c tried to slurp from illegal anthill %d\n",
	    elapsed(), aname, anthill); 
	sulk(aname); 
        return FALSE; 
    } else if (ants_left[anthill]==0) { 
	if (debug) 
	    printf(
		"%09.6f (debug) ERROR: aardvark %c tried to slurp from empty anthill %d\n",
		elapsed(), aname, anthill); 
        sulk(aname); 
	return FALSE; 
    } else if (is_jailed(aname)) { 
	if (!quiet) 
	    printf(
		"%09.6f CONCURRENCY BOTCH: aardvark %c is already in jail\n",
		elapsed(), aname); 
	return FALSE; 
    } else { 
        pthread_mutex_lock(&state_lock);
	if (slurping[anthill]>=AARDVARKS_PER_HILL) { 
	    if (debug) 
		printf(
		    "%09.6f (debug) ERROR: aardvark %c tried to slurp anthill %d where %d aardvarks were slurping\n",
		    elapsed(), aname, anthill, AARDVARKS_PER_HILL); 
	    pthread_mutex_unlock(&state_lock);
	    sulk(aname); 
	    return FALSE; 
	} else { 
	    pthread_mutex_unlock(&state_lock);
	    if (make_busy(aname)) { 
		if (!quiet) 
		    printf("%09.6f Aardvark %c slurping Anthill %d\n", 
			elapsed(), aname, anthill); 
		if (csv) 
		    fprintf(csv, 
			"%09.6f,%c,slurping,%d\n", 
			elapsed(), aname, anthill); 
		update_slurping(anthill, 1); 
		sleep(SLURP_TIME); // simulate external time passing
		update_slurping(anthill, -1); 

	    } else { 
		if (!quiet) 
			printf("%09.6f CONCURRENCY BOTCH: Aardvark %c already slurping! Jailed for %d seconds!\n",  elapsed(), aname, JAIL_TIME); 
		make_jailed(aname); 
		return FALSE; // no swallowing in this case
	    } 
	    make_idle(aname); 

	    // check whether you got an ant. This is a race condition. 
	    int gotit=lick(anthill); 
	    if (is_jailed(aname)) { 
		if (!quiet) 
		    printf("%09.6f Aardvark %c in jail for %d seconds\n", 
			elapsed(), aname, JAIL_TIME); 
		if (csv) 
		    fprintf(csv, 
			"%09.6f,%c,jailed,%d\n", 
			elapsed(), aname, anthill); 
		sleep(JAIL_TIME); 
		make_unjailed(aname); 
 	    } 
	    if (gotit) { 
		if (debug) 
		    printf(
		    "%09.6f (debug) Aardvark %c slurped an ant from anthill %d\n",
			elapsed(), aname, anthill); 
		swallow(aname); 
		return TRUE; 
	    } else { 
		if (debug) 
		    printf(
		    "%09.6f (debug) FAILURE: Aardvark %c slurped from empty anthill %d\n",
			elapsed(), aname, anthill); 
		// no swallow() or sulk() in this case: lost the race 
		return FALSE; 
	    } 
	} 
    } 
    fprintf(stderr,"ERROR: simulator failure: notify instructor!\n"); 
    return FALSE; 
} 

int main(int argc, char **argv)
{

   debug=FALSE; 
   int i; 
   for (i=1; i<argc; i++) { 
       if (strcmp(argv[i],"debug")==0) { 
	   debug=TRUE; 
       } else if (strcmp(argv[i],"quiet")==0) { 
	   quiet=TRUE; 
       } else if (strcmp(argv[i],"trace")==0) { 
	   trace=TRUE; 
       } else if (strcmp(argv[i],"csv")==0) { 
	   csv = fopen("output.csv", "w"); 
	   if (!csv) { 
		fprintf(stderr, "can't write output.csv\n"); 
		exit(1); 
  	   } 
       } else { 
	    fprintf(stderr, "%s usage: %s [debug] [trace] [quiet] [csv]\n", argv[0], argv[0]); 
	    fprintf(stderr, "  debug: print detailed debugging information\n"); 
	    fprintf(stderr, "  trace: print a trace of system state\n"); 
	    fprintf(stderr, "  quiet: suppress running narrative\n"); 
	    fprintf(stderr, "  csv: write state log to output.csv\n"); 
	    exit(1); 
       } 
   } 

   gettimeofday(&start,NULL); 

   char anames[AARDVARKS]; 	// names of aardvarks: A-G
   pthread_mutex_init(&state_lock, NULL);
   int anumber; 
   for (anumber=0; anumber<AARDVARKS; anumber++) { 
       anames[anumber]=ANAME(anumber); 
       pthread_create( &threads[anumber], NULL, 
                       thread_routines[anumber], (void *)(anames+anumber));
   } 
   for (anumber=0; anumber<AARDVARKS; anumber++) { 
       void *retptr; 
       pthread_join(threads[anumber],(void **)&retptr);
   } 
   pthread_mutex_destroy(&state_lock);
   double e = elapsed(); 
   printf("total elapsed real time is %09.6f seconds\n", elapsed()); 
   exit(0); 
}

