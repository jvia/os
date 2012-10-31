/**
 * Assignment 3 -- Sandboxing a Process
 * 2012-10-23
 *
 * COMPILLING: This program and be compiled with a call to
 * `make'. Running `make test' will run the program with each
 * misbehaving program in turn.
 * 
 * - - -
 * 
 * STACK CONDITION: Resource limits.

 * HEAP CONDTION: Resource limits.
 *
 * FORK CONDITION: Resource limits.
 *
 * CPUTIME CONDITION: Resource limits.
 * 
 * STDOUT CONDITION: Capture child's stdout and count the lines.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include <errno.h>
#include <signal.h>

#define BUF_SIZE 1024

void print_usage(int, struct rusage);
void* outcap(void*);

pid_t pid;  
int fd[2];
int count = 0;
pthread_t ou_daemon;


void main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "usage: watch <program>\n");
    exit(1);
  }

  // Make a pipe
  pipe(fd);

  if ((pid = fork())) {
    pthread_create(&ou_daemon, NULL, outcap, (void*) pid);
    struct rusage usage;
    int status;
    wait3(&status, 0, &usage);
    print_usage(status, usage);
  } else {
    // Limit child to 4MB of heap
    struct rlimit data;
    data.rlim_cur = 4000000;
    data.rlim_max = 4000000;
    // Limit stack memory of child to 4MB
    struct rlimit stack;
    stack.rlim_cur = 4000000;
    stack.rlim_max = 4000000;
    // Limit child to 20 forks
    struct rlimit nproc;
    nproc.rlim_cur = 20;
    nproc.rlim_max = 20;
    // Limit child to 1 CPU second
    struct rlimit cpu;
    cpu.rlim_cur = 1;
    cpu.rlim_max = 1;

    // Set the rlimits
    setrlimit(RLIMIT_DATA,  &data);
    setrlimit(RLIMIT_STACK, &stack);
    setrlimit(RLIMIT_NPROC, &nproc);
    setrlimit(RLIMIT_CPU,   &cpu);

    // Set up pipe betwen parent and child
    close(1);
    dup(fd[1]);
    close(fd[1]);
    close(fd[0]);

    // Run the program in the sandbox
    char* cmd[argc];
    for (int i = 0; i < argc; i++)
      cmd[i] = argv[i+1];
    cmd[argc] = NULL;
    execvp(cmd[0], cmd);
  }

  exit(0);
}

/**
 * Print end process information.
 */
void print_usage(int status, struct rusage usage)
{
  if (status != 0) fprintf(stderr, "Child killed (%d)\n", status);
  time_t unixtime;
  unixtime = time(NULL);
  printf("Runtime: %f\n", usage.ru_utime.tv_sec
         + usage.ru_stime.tv_usec
         + (usage.ru_utime.tv_usec / 1000.)
         + (usage.ru_stime.tv_usec / 1000.));
  printf("Lines Printed: %d \n", count);
  printf("Endtime: %s", ctime(&unixtime));
  printf("Status: %d\n", status);
}

/**
 * Monitor child output.
 */
void* outcap(void* _pid)
{
  pid_t pid = (int) _pid;  
  FILE* read = fdopen(fd[0], "r");

  while (!feof(read)) {
    if (++count >= 100) {
      kill(pid, SIGKILL);
      pthread_exit(NULL);
    }
    char buf[BUF_SIZE];
    fgets(buf, BUF_SIZE, read);
    printf("%s", buf);
  }

  pthread_exit(NULL);
}

