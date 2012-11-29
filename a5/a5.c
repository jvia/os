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

/** Use the random replacement algorithm. */
#define RDM (1 << 0)
/** Use a predictive pager. */
#define PRD (1 << 2)
/** Use a probabilistic state machine algorithm. */
#define PSM (1 << 3)
/** The pager to use. */
#define PAGER PSM

//////////////////////////////////////////////////////////////////////
// Proto declarations
int used_paged(Pentry[]);
int all_inactive(Pentry[]);
void initial_page(Pentry[]);
int most_likely_transition(int,int);
int pages_to_free(int);
int idle_pages(Pentry[]);
int free_pages(Pentry[]);
void random_pager(Pentry[]);
void predictive_pager(Pentry[]);
void psm_pager(Pentry[]);
int compdoubles(const void*, const void*);

//////////////////////////////////////////////////////////////////////
// Global variables

/** A transition table for each process, from page to page. */
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

/** The jump probability table. */
double prob_jump[20][20] = {
  {0.092130, 0.000926, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.091667, 0.000926, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.091667, 0.000926, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000093, 0.000000, 0.000000, 0.091667, 0.000579, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000162, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.073333, 0.000741, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.073333, 0.000741, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.073333, 0.000741, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.073333, 0.000741, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000046, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.073333, 0.000556, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000046, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.055000, 0.000394, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.055000, 0.000556, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000023, 0.000000, 0.000000, 0.000000, 0.000162, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.055000, 0.000370, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.036667, 0.000370, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000116, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.036667, 0.000185, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000139, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.018333, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000},
  {0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000, 0.000000}
};

// Globals
int tick = 0;
int init = 0;

/**
 * Deals with swapping pages in and out of memory.
 *
 * Depending on the startegy used, this function behaves differently.
 *
 * @param q the process states
 */
void pageit(Pentry q[MAXPROCESSES])
{
  switch (PAGER) {
  case RDM: random_pager(q);     break;
  case PRD: predictive_pager(q); break;
  case PSM: psm_pager(q);        break;
  }
}

/**
 * A pager which places a random page in memory.
 *
 * @param q the state of every process
 */
void random_pager(Pentry q[MAXPROCESSES]) {

  int proc, pc, page, oldpage;
  static int last_pages[MAXPROCESSES];
  
  // page in two pages for 1/2 of processes
  if (!init) {
    initial_page(q);
    init = 1;
    return;
  }

  for (proc = 0; proc < MAXPROCESSES; proc++) { 
    if (q[proc].active) { 

      pc = q[proc].pc; 
      page = pc / PAGESIZE;

      if (!q[proc].pages[page] && !pagein(proc,page))
        for (oldpage = 0; oldpage<q[proc].npages; oldpage++) 
          if (oldpage != page) 
            pageout(proc, oldpage);
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
    int i, j;
    for (i = 0; i < MAXPROCPAGES; ++i) {
      for (j = 0; j < MAXPROCPAGES; ++j) 
        printf("%d ", jump[i][j]);
      printf("\n");
    }
  }
}

/**
 * Predictive pager pre-fetches pages it thinks will be needed next.
 *
 * It does this by examining a process current page and paging in as
 * many pages it is likely to need. At all times it attempts to remove
 * as many idle pages as possible, ensuring thatthese are not pages
 * that are likely to be needed in the future.
 * 
 * @param q the process states
 */
void predictive_pager(Pentry q[MAXPROCESSES]) { 
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
            // If idle page and won't jump to that page, page it out
            for (int j = 0; j < MAXPROCPAGES; j++)
              if (q[i].pages[j] && j != curr_proc_page && !jump[curr_proc_page][j])
                  pageout(i, j);
            }
          }
      }
      // Page loaded succesfully; keep track in timestamps
      else {
        // Try to pagein all pages we can
        for (int p = 0; p < MAXPROCPAGES; p++)
          if (jump[page][p] && !pagein(proc, p))
            break;
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


/**
 * Similar to the predictive pager, except that it prioritizes by
 * transition probability.
 *
 * @param q the process states
 */
void psm_pager(Pentry q[MAXPROCESSES]) { 
  int proc, pc, page, oldpage, i, j;
  double tmp[MAXPROCPAGES];

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
          for (i = 0; i < MAXPROCESSES; i++) {
            int curr_proc_page = q[i].pc / PAGESIZE;
            for (j = 0; j < MAXPROCPAGES; j++)
              // Idle page with no transition probability
              if (q[i].pages[j] && j != curr_proc_page && !jump[curr_proc_page][j])
                  pageout(i, j);
          }
        }        
      }
      // Page loaded succesfully; keep track in timestamps
      else {
        // Sort the pages by likelihood, paging in all that aren't 0
        // as long as we can
        for (i = 0; i < MAXPROCPAGES; i++)
          tmp[i] = prob_jump[page][i];
        qsort(tmp, MAXPROCPAGES, sizeof(double), compdoubles);
        for (i = 0; i < MAXPROCPAGES; i++)
          if (jump[page][i] && !pagein(proc, i))
            break;
      }
    }
    // If process is inactive, free all its pages
    else {
      for (page = 0; page < MAXPROCPAGES; page++)
        pageout(proc, page);
    }
  }  
}

/**
 * Compare to doubles such that they are in decreasing order.
 *
 * @param a pointer to a double
 * @param b pointer to a double
 */
int compdoubles(const void *a, const void *b)
{
  const int *arg1 = a;
  const int *arg2 = b;

  if (*arg1 > *arg2) return  1;
  if (*arg1 < *arg2) return -1;
  return 0;
}

/**
 * Determines the number of used pages.
 *
 * @param q the process states
 */
int used_pages(Pentry q[MAXPROCESSES]) {
  int p, i, total = 0;
  for (p = 0; p < MAXPROCESSES; p++)
    for (i = 0; i < MAXPROCESSES; i++)
      total += q[p].pages[i];
  return total;
}

/**
 * Determines if there are any free pages.
 *
 * @param q the process states
 */
int free_pages(Pentry q[MAXPROCESSES])
{
  return !used_pages(q);
}

/**
 * Determines if all processes are inactive.
 *
 * @param q the process states
 */
int all_inactive(Pentry q[MAXPROCESSES])
{
  for (int proc = 0; proc < MAXPROCESSES; proc++)
    if (q[proc].active) return 0;
  return 1;
}

/**
 * Initial page load.
 *
 * The goal is to get as many processes running as possible. This
 * function simply loads two pages for 10 processes.
 *
 * @param q the process states
 */
void initial_page(Pentry q[MAXPROCESSES]) {
  int proc, pc, page;
  for (proc = 0; proc < 10; proc++) {
    // Unlikely to be true, but if so, we can pre-fetch for another
    // process
    if (!q[proc].active) {
      proc--;
      continue;
    }

    pc = q[proc].pc;
    page = pc / PAGESIZE;

    pagein(proc, page);
    pagein(proc, page + 1);
  }
}

/**
 * Calculate how many additional pages to free.
 *
 * @param from the page from which the pc may jump
 */
int pages_to_free(int from)
{
  int total = 0;
  for (int to = 0; to < MAXPROCPAGES; to++)
    total += jump[from][to];
}

/**
 * Calculate the number of idle pages.
 *
 * An idle page is one that is in memory but which is not being used
 * by any process. These pages are candidates for being paged
 * out. This is calculated by walking over the proc/page listing and
 * counting how many pages are in memory but not the current location
 * of the pc.
 *
 * @param q the state of the processes
 */
int idle_pages(Pentry q[MAXPROCESSES])
{
  int total = MAXPROCPAGES;
  int curr_page, proc;
  for (proc = 0; proc < MAXPROCESSES; proc++) {
    curr_page = q[proc].pc / PAGESIZE;
    if (q[proc].pages[curr_page])
      total--;
  }
  return total;
}

