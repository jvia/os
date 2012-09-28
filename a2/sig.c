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
#include <pthread.h>

// Function prototypes
void signal_handler(int);
clock_t program_time();
double ptime(void);
void add_line(const char*);
void get_lines(char*);

// Globals
char** buffer;
pthread_mutex_t buffer_lock;

int main(int argc, char** argv)
{
  // Setup mutex
  pthread_mutex_init(&buffer_lock, NULL);

  // setup signal handler
  struct sigaction act;
  act.sa_handler = signal_handler;
  sigaction(SIGALRM, &act, NULL);
  sigaction(SIGINT,  &act, NULL);
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
  char* line = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  while(1) {
    linelen = getline(&line, &linecap, stdin);
    if (linelen > 0) {
      pthread_mutex_lock(&buffer_lock);
      line[linelen-1] = '\0';
      printf("%s\n", line);
      pthread_mutex_unlock(&buffer_lock);
    }
  }
}


void get_lines(char* lines)
{
  // probably need to modify signal handler here to termporarily block
  // the signal to prevent the interrupt and thus a deadlock
  //struct sigaction cact;
  //sigaction(SIGTSTP, NULL, &cact);
  //sig
  //cact.sa_mask = SIGTSTP;
  //sigaction(SIGTSTP, &cact, NULL);
  int oldmask = sigsetmask(sigmask(SIGTSTP));
  pthread_mutex_lock(&buffer_lock);
  fprintf(stderr, "Sleeping\n");
  sleep(10);
  fprintf(stderr, "Done Sleeping\n");
  pthread_mutex_unlock(&buffer_lock);
  sigsetmask(oldmask);
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
    printf("IN SIGTSTP\n");
    get_lines(NULL);
    break;
  case SIGTERM:
    printf("Program time: %f\n", ptime());
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
