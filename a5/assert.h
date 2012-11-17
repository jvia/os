#ifndef ASSERT_H_DEFINED
#define ASSERT_H_DEFINED

#include <stdio.h>
#include <stdarg.h> 
#include <sys/types.h>

// shorthands for assertion handling
#define CHECK(bool)   check((bool),#bool,__FILE__,__LINE__)
#define ASSERT(bool)  assert((bool),#bool,__FILE__,__LINE__)
#define POSIT(bool)   posit((bool),#bool,__FILE__,__LINE__)
#define DIE(reason)   die((reason),__FILE__,__LINE__)
#define CARP(reason) carp((reason),__FILE__,__LINE__)

// always print the result of a test
inline void check(int boolean, char *boolstr, char *file, int line) { 
    if (!boolean) {
        fprintf(stderr,"ERROR: %s failed in line %d of file %s\n",
                boolstr,line,file);
    } else { 
	fprintf(stderr,"%s succeeded in line %d of file %s\n",
                boolstr,line,file);
    }
}

// die if an assertion fails. 
inline void assert(int boolean, char *boolstr, char *file, int line) {
    if (!boolean) {
        fprintf(stderr,"Assertion %s failed in line %d of file %s\n",
                boolstr,line,file);
        exit(1);        /* exit(0) means correct execution! */
    }
}

// report failing assertions without bombing out... keeps running
inline int posit(int boolean, char *boolstr, char *file, int line) {
    if (!boolean) 
        fprintf(stderr,"Assertion %s failed in line %d of file %s\n",
                boolstr,line,file);
    return boolean; 
}

// die on a fatal error
inline void die(char *condition, char *file, int line) {
    fprintf(stderr,"Fatal error: %s at line %d of file %s\n", 
        condition,line,file); 
    exit(1); 
}

// print a non-fatal error 
inline void carp(char *condition, char *file, int line) {
    fprintf(stderr,"Non-fatal error: %s at line %d of file %s\n",
	condition,line,file); 
}

#endif /* ASSERT_H_DEFINED */ 
