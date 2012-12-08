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
#include <sys/stat.h>
#include <unistd.h>

#define PAGE_SIZE  4096
#define BUFFER_SIZE 4096

#define TIMES 100

typedef struct _res {
  long time;
  long stddev;
} result;

//////////////////////////////////////////////////////////////////////
// Proto Declarations
long avg(long*,int);
long stddev(long*, int);

FILE *file;
char *buffer;
long file_size;


long avg(long *times, int len)
{
  long avg = 0L;
  int i;
  for (i = 0; i < len; i++) {
    avg += times[i];
  }
  return (avg / len);
}

long stddev(long *times, int len)
{
  long stddev = 0L;
  long u = avg(times, len);

  // calculate variance
  double var = 0L;
  int i;
  printf ("mean = %ld\n", u);
  for (i = 0; i < len; ++i) {
    printf("%2d => %ld; %ld; %f\n", i, times[i], times[i] - u, pow(times[i] - u, 2.0));
    var += pow(times[i] - u, 2.0);
  }
  var /= len;

  // calculate stddev
  stddev = sqrt(var);
 
  return stddev;
}

/**
 * Benchmark the disk read operation, no cache.
 *
 * @return return the result of the benchmark.
 */
result benchmark_disk_read(const char *path)
{
  struct timespec start, end;
  long times[TIMES];

  // prepare
  int fd = open(path, O_RDONLY, 0);
  buffer = malloc(BUFFER_SIZE * sizeof(char));

  // run becnhmark
  int i;
  for (i = 0; i < TIMES; ++i) {
    sync();
    clock_gettime(CLOCK_REALTIME, &start);
    read(fd, &buffer, BUFFER_SIZE);
    clock_gettime(CLOCK_REALTIME, &end);
    times[i] = end.tv_nsec - start.tv_nsec;
  }
    
  // clean up
  free(buffer);
  close(fd);

  // calculate
  result res = {0, 0};
  res.time = avg(times, TIMES);
  res.stddev = stddev(times, TIMES);
  return res;
}

/**
 * Benchmark the disk write operation.
 * 
 * @return return the result of the benchmark.
 */
result benchmark_disk_write(const char *path)
{
  struct timespec start, end;
  long times[TIMES];
  
  // prepare
  int fd = open(path, O_WRONLY, 0);
  buffer = malloc(BUFFER_SIZE * sizeof(char));

  // populate buffer for writing
  int i;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] = 'a';
  }

  // run becnhmark
  for (i = 0; i < TIMES; ++i) {
    sync();
    clock_gettime(CLOCK_REALTIME, &start);
    write(fd, &buffer, BUFFER_SIZE);
    clock_gettime(CLOCK_REALTIME, &end);
    times[i] = end.tv_nsec - start.tv_nsec;
  }

  // clean up
  free(buffer);
  close(fd);

  // calculate
  result res = {0, 0};
  res.time = avg(times, TIMES);
  res.stddev = stddev(times, TIMES);
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
  long times[TIMES];

  // open and read once to move into cache
  fd = open(path, O_RDONLY, 0);
  buffer = malloc(BUFFER_SIZE*sizeof(char));
  read(fd, &buffer, BUFFER_SIZE);

  // benchmark
  int i;
  for (i = 0; i < TIMES; ++i) {
    clock_gettime(CLOCK_REALTIME, &start);
    read(fd, &buffer, BUFFER_SIZE);
    clock_gettime(CLOCK_REALTIME, &end);
    times[i] = end.tv_nsec - start.tv_nsec;
  }

  // cleaup
  free(buffer);
  close(fd);

  res.time = avg(times, TIMES);
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

  // Stuff to print for fun, while debuggin
  printf ("\n--------------------\n");
  printf ("Page size: %d\n" , getpagesize());
  printf ("--------------------\n");
  
  printf ("Disk Read  \n  Average: %ld ns\n  Stddev:  %ld ns\n", disk_read.time,   disk_read.stddev);
  printf ("Disk Write \n  Average: %ld ns\n  Stddev:  %ld ns\n", disk_write.time,  disk_write.stddev);
  printf ("Cache Read \n  Average: %ld ns\n  Stddev:  %ld ns\n", cache_read.time,  cache_read.stddev);
  printf ("Cache Write\n  Average: %ld ns\n  Stddev:  %ld ns\n", cache_write.time, cache_write.stddev);
  
  exit(0);
}

