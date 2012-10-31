/**
 * Assignment 3 -- Sandboxing a Process
 * 2012-10-23
 *
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

#define BUF_SIZE 1024

void print_usage(int, struct rusage);
void* outcap(void*);


int fd[2];
int count = 0;

// TODO Add handler for SIGXCPU
// TODO Handle ENOMEM error bit
// TODO Handle EAGAIN
// TODO TODO Handle SIGSEGV

void main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "usage: watch <program>\n");
    exit(1);
  }

  pthread_t ou_daemon;
  pipe(fd);
 
  pid_t pid;  
  if ((pid = fork())) {
    pthread_create(&ou_daemon, NULL, outcap, (void*) pid);
    struct rusage usage;
    int status;
    wait3(&status, 0, &usage);
    print_usage(status, usage);
  } else {
    struct rlimit data;
    struct rlimit stack;
    struct rlimit nproc;
    struct rlimit cpu;

    // Limit child to 4MB of heap
    data.rlim_cur = 4000000;
    data.rlim_max = 4000000;

    // Limit stack memory of child to 4MB
    stack.rlim_cur = 4000000;
    stack.rlim_max = 4000000;

    // Limit child to 20 forks
    nproc.rlim_cur = 20;
    nproc.rlim_max = 20;

    // Limit child to 1 CPU second
    cpu.rlim_cur = 1;
    cpu.rlim_max = 1;

    // Set the rlimits
    setrlimit(RLIMIT_DATA,  &data);
    setrlimit(RLIMIT_STACK, &stack);
    setrlimit(RLIMIT_NPROC, &nproc);
    setrlimit(RLIMIT_CPU,   &cpu);

    close(1);
    dup(fd[1]);
    close(fd[1]);
    close(fd[0]);

    //////////////////////////////////////////////////////////////////////
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
 * When the child dies, its total runtime, number of lines printed to
 * stdout, and time of death should be reported. After any of these,
 * the watch program should exit. A regular exit from watch should
 * also report time spent by the child.
 */
void print_usage(int status, struct rusage usage)
{
  time_t unixtime;
  unixtime = time(NULL);
  printf("Runtime: %f\n", usage.ru_utime.tv_sec + usage.ru_stime.tv_usec
         + (usage.ru_utime.tv_usec / 1000.) + (usage.ru_stime.tv_usec / 1000.));
  printf("Lines Printed: %d \n", count);
  printf("Endtime: %s", ctime(&unixtime));
  printf("Status: %d\n", status);
}

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
    printf("parent: %s", buf);
  }

  pthread_exit(NULL);
}
