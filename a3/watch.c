/**
 * Assignment 3 -- Sandboxing a Process
 * 2012-10-23
 *
 * Part of the problem is to distinguish which behaviors can be
 * controlled through the operating system itself, which ones can be
 * controlled through monitoring, and which can only be controlled
 * partially due to extenuating circumstances. Some of the above
 * conditions are easy to assure, and some are impossible to
 * completely control. It is part of your task to determine which ones
 * are possible to accomplish.
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

void print_usage(int, struct rusage);

/**
   - Some child conditions can be detected via signals. Use them if at
   all possible! In particular, you will want to trap SIGCHLD in the
   parent and react accordingly!

   - Part of your program can be some form of "monitoring loop" in which
   the program repeatedly measures the behavior of the child.

   - Feel free to redirect I/O from the child to a subthread of your
   watch process.

   - You should presume that the child can do anything, including
   changing its behavior according to all available system calls. You
   cannot assume that the child won't try to get around your controls
   via malicious means.

   - In particular, note that you cannot install a signal handler in the
   child, because during an exec, that handler would get overwritten!

   - But the child is permitted to install its own signal handlers to
   thwart your controls, and you must be able to handle that!
*/

void main(int argc, char** argv)
{
  if (argc < 2) {
    fprintf(stderr, "usage: watch <program>\n");
    exit(1);
  }

  //////////////////////////////////////////////////////////////////////
  // Set rlimits
  /* struct rlimit o_stack; */
  /* struct rlimit o_data; */
  /* struct rlimit o_nproc; */
  /* struct rlimit o_cpu; */

  /* getrlimit(RLIMIT_STACK, &o_stack); */
  /* getrlimit(RLIMIT_DATA,  &o_data); */
  /* getrlimit(RLIMIT_NPROC, &o_nproc); */
  /* getrlimit(RLIMIT_CPU,   &o_cpu); */




  //////////////////////////////////////////////////////////////////////
  // Setup signal handler


  pid_t pid;
  if ((pid = fork())) {
    // Limit stack memory of child to 4MB
    struct rlimit stack;
    stack.rlim_cur = 4000000;
    stack.rlim_max = 4000000;
    prlimit(pid, RLIMIT_STACK, &stack, NULL);

    /*
      TODO If the child occupies more than 4 MB of heap memory, it
      should be killed and this event should be reported. The program
      2.c does this.
    */
    struct rlimit data;
    data.rlim_cur = 4000000;
    data.rlim_max = 4000000;
    prlimit(pid, RLIMIT_DATA, &data, NULL);

    /*
      TODO If the child forks more than 20 times, it should be killed
      and the event should be reported. The program 3.c does this.
    */
    struct rlimit nproc;
    nproc.rlim_cur = 20;
    nproc.rlim_max = 20;
    prlimit(pid, RLIMIT_NPROC, &nproc, NULL);

    /*
      TODO If the child uses more than 1 CPU-second of computer time, it
      should be killed and this event should be reported. The program
      4.c does this.
    */
    struct rlimit cpu;
    cpu.rlim_cur = 1;
    cpu.rlim_max = 1;
    prlimit(pid, RLIMIT_CPU, &cpu, NULL);

    /*
      TODO If the child produces more than 100 lines of output to
      stdout, it should be killed and this should be reported. The
      program 5.c does this.

      Will need to dup its IO.
    */


    // TODO You must forward everything the child prints to stdout, even if you capture it yourself.


    // WATCH PROCESS
    struct rusage usage;
    int status;
    wait3(&status, 0, &usage);
    print_usage(status, usage);
  } else {
    // TODO See if this can be made more elegant
    char* cmd[argc];
    for (int i = 0; i < argc; i++)
      cmd[i] = argv[i+1];
    cmd[argc] = NULL;
    execvp(cmd[0], cmd);
  }

  exit(0);
}

/**
 * TODO When the child dies, its total runtime, number of lines printed
 * to stdout, and time of death should be reported. After any of
 * these, the watch program should exit. A regular exit from watch
 * should also report time spent by the child.
 *
 */
void print_usage(int status, struct rusage usage)
{
  time_t unixtime;
  unixtime = time(NULL);
  printf("Runtime: %f\n", usage.ru_utime.tv_sec + usage.ru_stime.tv_usec
         + (usage.ru_utime.tv_usec / 1000.) + (usage.ru_stime.tv_usec / 1000.));
  printf("Lines Printed: %ld \n", usage.ru_oublock);
  printf("Endtime: %s", ctime(&unixtime));
  printf("Status: %d\n", status);
}
