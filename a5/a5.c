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
#define PAGER PRD

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


// Globals
int tick = 0;

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
  case RDM: return random_pager(q);
  case PRD: return predictive_pager(q);
  case PSM: return psm_pager(q);
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

/**
 * Similar to the predictive pager, except that it prioritizes by
 * transition probability.
 *
 * @param q the process states
 */
void psm_pager(Pentry q[MAXPROCESSES]) { 
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

/**
 * Determines the number of used pages.
 *
 * @param q the process states
 */
int used_pages(Pentry q[MAXPROCESSES]) {
  int p, i, total = 0;
  for (p = 0; p < MAXPROCESSES; p++) {
    for (i = 0; i < MAXPROCESSES; i++) {
      total += q[p].pages[i];
    }
  }
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
  for (int proc = 0; proc < MAXPROCESSES; proc++) {
    if (q[proc].active) return 0;
  }
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
  for (proc = 0; proc < 10 / 2; proc++) {
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
    timestamps[proc][page] = tick;
    timestamps[proc][page + 1] = tick;
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
  for (int to = 0; to < MAXPROCPAGES; to++) {
    total += jump[from][to];
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
 *
 * @param q the state of the processes
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

