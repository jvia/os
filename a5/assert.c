#include <stdio.h> 
#include "assert.h"
main() { 
   CARP("Nietsche is dead"); 
   CHECK(0==0); 
   CHECK(1==0); 
   POSIT(0==0); 
   POSIT(1==0); 
   ASSERT(0==0); 
   ASSERT(1==0); 
   DIE("can't live free"); 
} 
