/**
 * @file
 * 
 *
 * @author Jeremiah Via <jeremiah@cs.tufts.edu>
 * @version 2012-12-10
 */
#include <stdio.h>
#include <stdlib.h>

typedef struct _res {
  double time;
  double stddev;
} result;

FILE *file;

/**
 * Benchmark the disk read operation, no cache.
 *
 * @return return the result of the benchmark.
 */
result benchmark_disk_read(void)
{
  result res;
  return res;
}

/**
 * Benchmark the disk write operation.
 * 
 * @return return the result of the benchmark.
 */
result benchmark_disk_write(void)
{
  result res;
  return res;
}

/**
 * Benchmark the cache read.
 *
 * @return return the result of the benchmark.
 */
result benchmark_cache_read(void)
{
  result res;
  return res;
}

/**
 * Benchmark the cache write.
 * 
 * @return return the result of the benchmark.
 */
result benchmark_cache_write(void)
{
  result res;
  return res;
}

int main(int argc, char **argv)
{

  if (argc != 2) {
    fprintf(stderr, "usage: %s <file>\n", argv[0]);
    exit(1);
  }
  
  result disk_read   = benchmark_disk_read();
  result disk_write  = benchmark_disk_write();
  result cache_read  = benchmark_cache_read();
  result cache_write = benchmark_cache_write();
  
  printf ("Disk Read\n  Average: %f\n  Stddev:  %f\n",   disk_read.time,   disk_read.stddev);
  printf ("Disk Write\n  Average: %f\n  Stddev:  %f\n",  disk_write.time,  disk_write.stddev);
  printf ("Cache Read\n  Average: %f\n  Stddev:  %f\n",  cache_read.time,  cache_read.stddev);
  printf ("Cache Write\n  Average: %f\n  Stddev:  %f\n", cache_write.time, cache_write.stddev);
  
  exit(0);
}
