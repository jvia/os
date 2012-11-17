#include <stdio.h> 
#include "t5.h"

// simple algorithm does one job at a time
// with miserable parallelism
// this is called BY ME every time something interesting occurs. 
//   q: state of every process. 

void pageit(Pentry q[MAXPROCESSES]) { 
// these are globals that only the subroutine can see
   static int tick=0; // artificial time
   static int timestamps[MAXPROCESSES][MAXPROCPAGES]; 
// these are regular dynamic variables on stack
   int proc,pc,page,oldpage; 

   // select first active process 
   for (proc=0; proc<MAXPROCESSES; proc++) { 
      // if active, then work on it ONLY 
      if (q[proc].active) { 
	 pc=q[proc].pc; 		// program counter for process
         page = pc/PAGESIZE; 		// page the program counter needs
	 timestamps[proc][page]=tick;	// last access
         if (!q[proc].pages[page]) { 	// if page is not there: 
	    if (!pagein(proc,page)) {   // try to swap in, if this fails: 
	       // look at all old pages, swap out any other pages 
	       for (oldpage=0; oldpage<q[proc].npages; oldpage++) {
		  // if I find a page that's not equal to the one I want, 
	          // swap it out => 100 ticks later, pagein will succeed. 
		  if (oldpage!=page && pageout(proc,oldpage)) break; 
                  //                   ^^^^^^^^^^^^^^^^^^^^^ swapout starts
                  //  ^^^^^^^^^^^^^ it's not the page I want
 		} 
	    } 
	 }
	 break; // no more 
      } 
   } 	
   tick++; // advance time for next iteration
} 

// proc: process to work upon (0-19) 
// page: page to put in (0-19)
// returns
//   1 if pagein started or already started
//   0 if it can't start (e.g., swapping out) 
// int pagein(int proc, int page); 

// proc: process to work upon (0-19)
// page: page to swap out. 
// returns: 
//   1 if pageout started (not finished!) 
//   0 if can't start (e.g., swapping in)
// int pageout(int proc, int page); 

// first strategy: least-recently-used
// make a time variable, count ticks. 
// make an array: what time each page was used for 
// each process: 
// int usedtime[PROCESSES][PAGES]; 
// At any time you need a new page, want to swap 
// out the page that has least usedtime. 

// second strategy: predictive
// track the PC for each process over time
// e.g., in a ring buffer. 
// int pc[PROCESSES][TIMES]; 
// at any time pc[i][0]-pc[i][TIMES-1] are the last TIMES
// locations of the pc. If these are near a page border (up 
// or down, and if there is an idle page, swap it in. 

// third strategy: probabilistic state machine
// From each page, compute the pages that you can branch to. 
// Swap them in according to their probability of occurrence. 

// Note: all data keeping must be in GLOBAL variables. 
