/**
 * @file
 *
 * Implementation of a pager.
 *
 * @author Jeremiah Via <jeremiah@cs.tufts.edu>
 * @version 2012-11-29
 *
 * @section DESCRIPTION
 *
 * Assignment 5 --- Paging
 */
#include <stdio.h>
#include <stdlib.h>

#include "t5.h"

/** @def RDM
 *  Use the random replacement algorithm.
 */
#define RDM (1 << 0)
#define LRU (1 << 1) // @def LRU use the LRU replacement algorithm
#define PRD (1 << 2) // @def PRD use a predictive replacement algorithm
#define PSM (1 << 3) // @def PSM use a probabilistic state machine algorithm

#define PAGER PSM

typedef struct _procpage {
  int proc;
  int page;
  int time;
} procpage;

// A transition table for each process, from page to page.
int jump[MAXPROCPAGES][MAXPROCPAGES] =
{
  {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0},
  {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

// Proto declarations
int used_paged(Pentry[]);
int all_inactive(Pentry[]);
void initial_page(Pentry[]);
int most_likely_transition(int,int);
int pages_to_free(int); // given a page, return numebr of pages with transition
int idle_pages(Pentry[]);
int free_pages(Pentry[]);

// Globals
int tick = 0;

#if PAGER == RDM
/**
 * A pager which places a random page in memory.
 *
 * @param q the state of every process
 */
void pageit(Pentry q[MAXPROCESSES]) { 
  int proc, pc, page, oldpage;
  static int last_pages[MAXPROCESSES];
  
  // page in two pages for 1/2 of processes
  if (tick == 0) {
    initial_page(q);
    tick++;
    return;
  }

  for (proc = 0; proc < MAXPROCESSES; proc++) { 
    if (q[proc].active) { 

      pc = q[proc].pc; 
      page = pc / PAGESIZE;

      if (!q[proc].pages[page]) {
        if (!pagein(proc,page)) {
          for (oldpage = 0; oldpage<q[proc].npages; oldpage++) {
            if (oldpage != page) {
              pageout(proc, oldpage);
            }
          }
        }
      }
    } 
  }

  // Computer transitions
  for (proc = 0; proc < MAXPROCESSES; proc++) {
    int curr_page = q[proc].pc / PAGESIZE;
    int prev_page = last_pages[proc];
    jump[prev_page][curr_page] = 1;
    last_pages[proc] = curr_page;
  }

  // If done, let's print table
  if (all_inactive(q)) {
    for (int i = 0; i < MAXPROCPAGES; ++i) {
        for (int j = 0; j < MAXPROCPAGES; ++j) {
            printf("%d ", jump[i][j]);
        }
        printf("\n");
    }
  }
} 
#elif PAGER == LRU
/**
 * Least-Recently-Used: make a time variable, count ticks. Make an
 * array: what time each page was used for each process.
 *
 * @param q the process state
 */
void pageit(Pentry q[MAXPROCESSES]) { 
  
  static int proc_time[MAXPROCESSES];

  int proc, pc, page, oldpage;

  // Initial page load
  if (tick == 0) {
    initial_page(q);
    tick++;
    return;
  }


  for (proc = 0; proc < MAXPROCESSES; proc++) { 
    if (q[proc].active) { 

      pc = q[proc].pc; 
      page = pc / PAGESIZE;

      if (!q[proc].pages[page]) {
        if (!pagein(proc,page)) {
          // Need to pageout an old page
          procpage lru = {0, 0, 0};
          for (int i = 0; i < MAXPROCESSES; ++i) {
            for (int j = 0; j < MAXPROCPAGES; ++j) {
              if (lru.time < timestamps[i][j] && q[i].pages[j]) {
                lru.proc = i;
                lru.page = j;
                lru.time = timestamps[i][j];
              }
            }
          }

          pageout(lru.proc, lru.page);
          //break;
        }
      }
    } 
  }

  // Update page usage time by determing which page each process is
  // currently using and resetting its value back to 0, while
  // increasing each other page.
  for (proc = 0; proc < MAXPROCESSES; proc++) {
    int cur_page = q[proc].pc / PAGESIZE;
    for (page = 0; page < MAXPROCPAGES; page++) {
      if (page == cur_page)
        timestamps[proc][page] = 0;
      else
        timestamps[proc][page]++;
    }
  }

  
  // Print out ifo at end
  if (all_inactive(q)) {
    for (int i = 0; i < MAXPROCPAGES; ++i) {
      for (int j = 0; j < MAXPROCPAGES; ++j) {
        printf ("%6d ", timestamps[i][j]);
      }
      printf ("\n");
    }
  }

  tick++;
} 



#elif PAGER == PRD
// second strategy: predictive
// track the PC for each process over time
// e.g., in a ring buffer. 
// int pc[PROCESSES][TIMES]; 
// at any time pc[i][0]-pc[i][TIMES-1] are the last TIMES
// locations of the pc. If these are near a page border (up 
// or down, and if there is an idle page, swap it in. 
void pageit(Pentry q[MAXPROCESSES]) { 
  //////////////////////////////////////////////////////////////////////
  // persistent function variables
  static int tick = 0; // artificial time
  static int timestamps[MAXPROCESSES][MAXPROCPAGES]; 

  // these are regular dynamic variables on stack
  int proc,pc,page,oldpage; 

  // select first active process 
  for (proc = 0; proc < MAXPROCESSES; proc++) { 
    if (q[proc].active) { 
      pc = q[proc].pc;
      page = pc/PAGESIZE;
      timestamps[proc][page] = tick;
      if (!q[proc].pages[page]) {
        if (!pagein(proc,page)) {
          for (oldpage = 0; oldpage<q[proc].npages; oldpage++) {
            if (oldpage!=page && pageout(proc,oldpage)) break; 
          } 
        } 
      }
      break; // no more 
    } 
  } 	
  tick++; // advance time for next iteration
}

#elif PAGER == PSM
// third strategy: probabilistic state machine
// From each page, compute the pages that you can branch to. 
// Swap them in according to their probability of occurrence. 
void pageit(Pentry q[MAXPROCESSES]) { 
  int proc, pc, page, oldpage;

  // page in two pages for 1/2 of processes
  if (tick == 0) {
    tick++;
    initial_page(q);
    return;
  }

  tick++;
  for (proc = 0; proc < MAXPROCESSES; proc++) { 
    if (q[proc].active) { 

      pc = q[proc].pc; 
      page = pc / PAGESIZE;

      // If desired page is not in memory, try to page it in. On
      // failure we head into the if-statement to remove unneeded
      // pages
      if (!q[proc].pages[page] && !pagein(proc,page)) {

        // Remove all low transition idle page
        if (idle_pages(q) && !free_pages(q)) {
          for (int i = 0; i < MAXPROCESSES; i++) {
            int curr_proc_page = q[i].pc / PAGESIZE;
            for (int j = 0; j < MAXPROCPAGES; j++) {
              if (q[i].pages[j] && j != curr_proc_page) { // idle page
                if (!jump[curr_proc_page][j]) { // low transtion prob
                  pageout(i, j);
                }
              }
            }
          }
        }        
      }
      // Page loaded succesfully; keep track in timestamps
      else {
        // Try to pagein all pages we can
        for (int p = 0; p < MAXPROCPAGES; p++)
        {
          if (jump[page][p]) {
            //printf ("PROC %2d pagein attemp with page %2d\n", proc, p);
            if (!pagein(proc, p)) break;
          }
        }

        timestamps[proc][page] = tick;
      }
    }
    // If process is inactive, free all its pages
    else {
      for (page = 0; page < MAXPROCPAGES; page++) {
        pageout(proc, page);
      }
    }
  }  
} 
#endif

int used_pages(Pentry q[MAXPROCESSES]) {
  int p, i, total = 0;
  for (p = 0; p < MAXPROCESSES; p++) {
    for (i = 0; i < MAXPROCESSES; i++) {
      total += q[p].pages[i];
    }
  }
  return total;
}

int free_pages(Pentry q[MAXPROCESSES])
{
  return !used_pages(q);
}

int all_inactive(Pentry q[MAXPROCESSES])
{
  for (int proc = 0; proc < MAXPROCESSES; proc++) {
    if (q[proc].active) return 0;
  }
  return 1;
}

/**
 * Initial page load.
 *
 * The goal is to get as many processes running as possible.
 */
void initial_page(Pentry q[MAXPROCESSES]) {
  int proc, pc, page;
  for (proc = 0; proc < MAXPROCESSES; proc++) {
    if (!q[proc].active) {
      proc--;
      continue;
    }

    pc = q[proc].pc;
    page = pc / PAGESIZE;

    pagein(proc, page);
    pagein(proc, page + 1);
    timestamps[proc][page] = tick;
    timestamps[proc][page + 1] = tick;
  }
}

int most_likely_transition(int proc, int page)
{
  int max_prob, max_page;
}

/**
 * Calculate how many additional pages to free.
 */
int pages_to_free(int page)
{
  int total = 0;
  for (int p = 0; p < MAXPROCPAGES; p++) {
    total += jump[page][p];
  }
}

/**
 * Calculate the number of idle pages.
 *
 * An idle page is one that is in memory but which is not being used
 * by any process. These pages are candidates for being paged
 * out. This is calculated by walking over the proc/page listing and
 * counting how many pages are in memory but not the current location
 * of the pc.
 */
int idle_pages(Pentry q[MAXPROCESSES])
{
  int total = MAXPROCPAGES;
  int curr_page;
  for (int proc = 0; proc < MAXPROCESSES; proc++) {
    curr_page = q[proc].pc / PAGESIZE;
    if (q[proc].pages[curr_page])
      total--;
  }
  return total;
}
