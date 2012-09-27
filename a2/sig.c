/*
  Assignment 02 -- processes and signals
  Jeremiah Via <jeremiah@cs.tufts.edu>
  2012-10-09
*/
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/resource.h>

void signal_handler(int);
clock_t program_time();
double ptime(void);

int main(int argc, char** argv)
{
  // setup signal handler
  struct sigaction act;
  act.sa_handler = signal_handler;
  sigaction(SIGALRM, &act, NULL);
  sigaction(SIGINT, &act, NULL);
  sigaction(SIGTERM, &act, NULL);
  sigaction(SIGTSTP, &act, NULL);


  // setup alarm
  struct itimerval timer;
  timer.it_interval.tv_sec = 10;
  timer.it_interval.tv_usec = 0;
  timer.it_value.tv_sec = 10;
  timer.it_value.tv_usec = 0;
  setitimer(ITIMER_REAL, &timer, NULL);

  // wait: http://www.gnu.org/software/libc/manual/html_node/Sigsuspend.html#Sigsuspend
  while(1);
}

void signal_handler(int status)
{
  static int count = 0;


  switch (status) {
  case SIGALRM:
    printf("tick %d...\n", count);
    count+=10;
    break;
  case SIGINT:
    printf("Program time: %f\n", ptime());
    break;
  case SIGTSTP:
  case SIGTERM:
    printf("Program time: %f\n", ptime());
  default:
    exit(0);
  }
}

double ptime()
{
  static struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  double time = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
  time += (usage.ru_utime.tv_usec / 1000.0) + (usage.ru_stime.tv_usec / 1000.0);
  return time;
}


clock_t program_time()
{
  struct tms timebuf;
  times(&timebuf);
  return timebuf.tms_utime + timebuf.tms_stime;
}
