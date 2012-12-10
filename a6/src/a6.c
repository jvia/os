/**
 * @file
 * 
 *
 * @author Jeremiah Via <jeremiah@cs.tufts.edu>
 * @version 2012-12-10
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/file.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>


#define BUFFER_SIZE 100

/**
 * Benchmark the disk read operation, no cache.
 *
 * @return return the result of the benchmark.
 */
long benchmark_disk_read(const char *path)
{
  struct timespec start, end;
  int fd = open(path, O_RDONLY, 0);
  char buffer[BUFFER_SIZE];

  sync();
  
  clock_gettime(CLOCK_REALTIME, &start);
  read(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
  close(fd);

  return end.tv_nsec - start.tv_nsec;
}

/**
 * Benchmark the disk write operation.
 *
 * n.b. The value this gets it probably inaccurate due to the fact
 * that the OS writes to the disk lazily, so data is most likely
 * sitting in the kernel buffer.
 *
 * @return return the result of the benchmark.
 */
long benchmark_disk_write(const char *path)
{
  struct timespec start, end;
  
  /* prepare */
  int fd = open(path, O_WRONLY, 0);
  char buffer[BUFFER_SIZE];

  /* populate buffer for writing */
  int i;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] = 'a';
  }
  
  sync();

  /* run becnhmark */
  clock_gettime(CLOCK_REALTIME, &start);
  write(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);

  /* clean up */
  close(fd);
  return end.tv_nsec - start.tv_nsec;
}

/**
 * Benchmark the cache read.
 *
 * @return return the result of the benchmark.
 */
long benchmark_cache_read(const char *path)
{
  struct timespec start, end;

  /* prepare */
  int fd = open(path, O_RDONLY, 0);
  char buffer[BUFFER_SIZE];

  sync();
  
  /* run becnhmark */
  read(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &start);
  read(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
    
  /* clean up */
  close(fd);
  return end.tv_nsec - start.tv_nsec;
}

/**
 * Benchmark the cache write.
 * 
 * @return return the result of the benchmark.
 */
long benchmark_cache_write(const char *path)
{
  struct timespec start, end;
  
  /* prepare */
  int fd = open(path, O_WRONLY, 0);
  char buffer[BUFFER_SIZE];
  
  /* populate buffer for writing */
  int i;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    buffer[i] = 'a';
  }

  sync();
  
  /* run becnhmark */
  clock_gettime(CLOCK_REALTIME, &start);
  write(fd, &buffer, BUFFER_SIZE);
  clock_gettime(CLOCK_REALTIME, &end);
  
  /* clean up */
  close(fd);
  return end.tv_nsec - start.tv_nsec;
}

/**
 * Purple draaaaank.
 */
int main(int argc, char **argv)
{
  long disk_read;
  long disk_write;
  long cache_read;
  long cache_write;

  if (argc != 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    exit(1);
  }

  disk_read   = benchmark_disk_read(argv[1]);
  disk_write  = benchmark_disk_write(argv[1]);
  cache_read  = benchmark_cache_read(argv[1]);
  cache_write = benchmark_cache_write(argv[1]);
  
  printf ("Disk Read:   %7ld ns\n", disk_read);
  printf ("Disk Write:  %7ld ns (untrustworthy)\n", disk_write);
  printf ("Cache Read:  %7ld ns\n", cache_read);
  printf ("Cache Write: %7ld ns\n", cache_write);
  
  exit(0);
}
