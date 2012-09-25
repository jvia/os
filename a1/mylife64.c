/*
  Assignment 01 -- Cheating the OS
  Jeremiah Via <jeremiah@cs.tufts.edu>
  2012-09-25

  Exploit based off of the paper "Secretly Monopolizing the CPU
  Without Superuser Privileges" by Dan Tsafrir, et al.
*/
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/times.h>
#include <time.h>

// Definitions
#define BUFSIZE  300
#define FRACTION 0.75
typedef unsigned long cycle_t;

// Globals
int **grid = NULL;
int **neighbors = NULL;
struct timespec zero = {0,0};

// Function prototypes
cycle_t cycles_per_tick();
inline cycle_t get_cycles();
int** make_grid(int, int);
void zero_grid(int**, int, int);
int** read_grid(FILE*, int*, int*);
void print_grid(FILE*, int**, int, int);
void free_grid(int**, int);
void next(int**, int, int);


int main(int argc, char **argv)
{
  // startup & error checking
  int rows = 0, cols = 0, iterations = 0;
  if (argc < 2 ||
      !(grid = read_grid(stdin, &rows, &cols)) ||
      (iterations = atoi(argv[1])) < 0) {
    fprintf(stderr,"life usage: life iterations < inputfile\n");
    exit(1);
  }

  // calculate how long to run between sleeps & synchronize
  cycle_t work, tick_start, now;
  work = FRACTION * cycles_per_tick();
  nanosleep(&zero, NULL);
  tick_start = get_cycles();

  // main loop with exploit
  int i;
  neighbors = make_grid(rows, cols);
  for (i = 0; i < iterations; i++) {
    now = get_cycles();
    if (now - tick_start >= work) {
      nanosleep(&zero, NULL);
      tick_start = get_cycles();
    }
    next(grid, rows, cols);
  }

  // printing & cleanup
  print_grid(stdout,grid,rows,cols); free_grid(grid,rows);
  free_grid(neighbors,rows);
}


/*
 * Calculate the number of cycles per tick using the high resolution
 * clock.
 */
cycle_t cycles_per_tick()
{
  int i;
  cycle_t start, finish, elapsed;
  const cycle_t hundred = 100;// number of trials

  // sync with accounting tick, then record cycles
  nanosleep(&zero,NULL);
  start = get_cycles();

  // sleep through 100 account ticks
  for(i=0 ; i<hundred ; i++)
    nanosleep(&zero,NULL);

  // record time & record the average number of cycls
  finish = get_cycles();
  elapsed = finish - start;
  elapsed &= 0xffffffff;
  return elapsed / hundred;
}


/*
 * Get number of elapsed cycles, using assembler for speed.
 */
inline cycle_t get_cycles()
{
  cycle_t ret;
  asm volatile("rdtsc" : "=A" (ret));
  return ret;
}


/*
 * Make game of life grid.
 */
int** make_grid(int rows, int cols) {
  int** out = (int**) malloc(rows * sizeof(int*));

  int r;
  for (r = 0; r < rows; r++)
    out[r] = (int *)malloc (cols * sizeof(int));

  return out;
}

/*
 * Make all cells non-living.
 */
void zero_grid(int **cells, int rows, int cols) {
  int r, c;
  for (r = 0; r < rows; r++)
    for (c = 0; c < cols; c++)
      cells[r][c]=0;
}

/*
 * Read a grid of cells from a text file.
 */
int** read_grid(FILE* f, int* rows, int* cols) {

  char buffer[BUFSIZE];
  fgets(buffer, BUFSIZE, f);
  while (buffer[0] == '#')
    fgets(buffer, BUFSIZE, f);

  if (sscanf(buffer, "x = %d, y = %d", cols, rows) == 2) {
    int** grid = make_grid(*rows, *cols);
    int r=0;
    zero_grid(grid, *rows, *cols);

    while (!feof(f) && r < (*rows)) {
      int c;
      fgets(buffer, BUFSIZE, f);
      for (c = 0; c < BUFSIZE && c < (*cols) &&
             buffer[qc] != '\n' && buffer[c] != '\0'; ++c) {
        if (buffer[c]=='.' || buffer[c]==' ')
          grid[r][c]=0;
        else
          grid[r][c]=1;
      }
      ++r;
    }
    return grid;
  } else {
    fprintf(stderr, "first line does not contain grid dimensions\n");
    return NULL;
  }
}

/* Print a grid in a form that can be read back in. */
void print_grid(FILE* fp, int** g, int rows, int cols) {
  int r, c;
  fprintf(fp, "x = %d, y = %d\n", cols, rows);
  for (r = 0; r < rows; r++) {
    for (c = 0; c < cols; c++)
      fprintf(fp, (g[r][c] ? "*" : "."));
    fprintf(fp,"\n");
  }
}

/*
 * Free the memory.
 */
void free_grid(int **grid, int rows) {
  int i;
  for (i=0; i<rows; i++)
    free(grid[i]);
  free(grid);
}

/*
 * Next iteration of the cellular automaton.
 */
void next(int **cells, int rows, int cols) {
  int r, c;

  for (r=0; r<rows; r++) {
    for (c=0; c<cols; c++) {
      int n = 0;
      if (c>0 && cells[r][c-1]) n++;
      if (r>0 && c>0 && cells[r-1][c-1]) n++;
      if (r>0 && cells[r-1][c]) n++;
      if (r>0 && c<cols-1 && cells[r-1][c+1]) n++;
      if (c<cols-1 && cells[r][c+1]) n++;
      if (r<rows-1 && c<cols-1 && cells[r+1][c+1]) n++;
      if (r<rows-1 && cells[r+1][c]) n++;
      if (r<rows-1 && c>0 && cells[r+1][c-1]) n++;
      neighbors[r][c]=n;
    }
  }
  
  for (r=0; r<rows; r++) {
    for (c=0; c<cols; c++) {
      /* any live cell with < 2 or > 3 neighbors dies */
      if (cells[r][c] && neighbors[r][c]<2 || neighbors[r][c]>3)
        cells[r][c] = 0;
      /* any dead cell with three neighbors lives */
      else if (!cells[r][c] && neighbors[r][c]==3)
        cells[r][c] = 1;
    }
  }
}
