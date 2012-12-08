/**
 * @file
 * 
 *
 * @author Jeremiah Via <jeremiah@cs.tufts.edu>
 * @version 2012-12-10
 */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define PAGE_SIZE  8096
#define WRITE_SIZE PAGE_SIZE*100
#define BUFFER_SIZE PAGE_SIZE

#define TIMES 100

typedef struct _res {
  long time;
  long stddev;
} result;

//////////////////////////////////////////////////////////////////////
// Proto Declarations
double avg(double*,int);

FILE *file;
char *buffer;

double to_secs(struct timeval start, struct timeval end) {
  return ((double) end.tv_sec - start.tv_sec)
    +    ((double) (end.tv_usec - start.tv_usec) / 1000.);
}

/**
 * Benchmark the disk read operation, no cache.
 *
 * @return return the result of the benchmark.
 */
result benchmark_disk_read(const char *path)
{
  result res = {0, 0} ;
  int fd;
  struct timespec start, end;
  double times[TIMES];

  // prepare
  fd = open(path, O_RDONLY, 0);
  buffer = malloc(BUFFER_SIZE*sizeof(char));

  // run
  int i;
  clock_gettime(CLOCK_REALTIME, &start);
  read(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
    
  // clean up
  free(buffer);
  close(fd);

  // calculate
  res.time = end.tv_nsec - start.tv_nsec;
  return res;
}

/**
 * Benchmark the disk write operation.
 * 
 * @return return the result of the benchmark.
 */
result benchmark_disk_write(const char *path)
{
  result res = {0, 0};
  int fd = open(path, O_WRONLY, 0);
  int i;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] = 'a';
  }

  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
  write(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
  res.time = end.tv_nsec - start.tv_nsec;
  close(fd);
  return res;
}

/**
 * Benchmark the cache read.
 *
 * @return return the result of the benchmark.
 */
result benchmark_cache_read(const char *path)
{
  result res = {0, 0};
  int fd;
  struct timespec start, end;
  double time[TIMES];
  
  fd = open(path, O_RDONLY, 0);
  buffer = malloc(BUFFER_SIZE*sizeof(char));
  read(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &start);
  read(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
  free(buffer);
  res.time = end.tv_nsec - start.tv_nsec;
  close(fd);
  return res;
}

/**
 * Benchmark the cache write.
 * 
 * @return return the result of the benchmark.
 */
result benchmark_cache_write(const char *path)
{
  result res = {0, 0};
  int fd = open(path, O_WRONLY, 0);
  int i;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] = 'a';
  }

  struct timespec start, end;
  clock_gettime(CLOCK_REALTIME, &start);
  write(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
  res.time = end.tv_nsec - start.tv_nsec;
    //((double) end.tv_sec - start.tv_sec) + ((double) (end.tv_nsec - start.tv_nsec) / 1000000000.);
  close(fd);
  return res;
}

/**
 * Randomly read/write to play around with the cache and prime it for
 * another benchmark.
 */
void play_with_cache(void)
{
  /* int fd = open("testfile.dat", O_RDONLY, 0); */
  /* buffer = malloc(BUFFER_SIZE * sizeof(char)); */

  /* int i; */
  /* for (i = 0; i < 10; ++i) { */
    
  /*   lseek(fd,  */
  /* } */
}

int main(int argc, char **argv)
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    exit(1);
  }
  
  result disk_read   = benchmark_disk_read(argv[1]);
  result disk_write  = benchmark_disk_write(argv[1]);
  result cache_read  = benchmark_cache_read(argv[1]);
  result cache_write = benchmark_cache_write(argv[1]);
  
  printf ("Disk Read  \n  Average: %ld ns\n  Stddev:  %ld ns\n", disk_read.time,   disk_read.stddev);
  printf ("Disk Write \n  Average: %ld ns\n  Stddev:  %ld ns\n", disk_write.time,  disk_write.stddev);
  printf ("Cache Read \n  Average: %ld ns\n  Stddev:  %ld ns\n", cache_read.time,  cache_read.stddev);
  printf ("Cache Write\n  Average: %ld ns\n  Stddev:  %ld ns\n", cache_write.time, cache_write.stddev);
  
  exit(0);
}

