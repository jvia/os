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
double ptime();
void add_line(const char*);
void print_lines();

// Globals
#define BUFFER_SIZE 10
char* buffer[10];
int frnt = 0, back = 0; // frnt is older than back
int hist = 0;
pthread_mutex_t buffer_lock;


int main(void)
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

  // Capture user input and place into ring buffer
  while(1) {
    char* line = NULL;
    size_t linecap = 0;
    ssize_t linelen;
    if ((linelen = getline(&line, &linecap, stdin)) > 0) {
      pthread_mutex_lock(&buffer_lock);
      line[linelen-1] = '\0';
      buffer[back] = line;
      back = (back + 1) % BUFFER_SIZE;
      ++hist;
      pthread_mutex_unlock(&buffer_lock);
    }
  }
}


void print_lines()
{
  int oldmask = sigsetmask(sigmask(SIGTSTP));
  int i, h;
  pthread_mutex_lock(&buffer_lock);
  printf("FRONT: %d, BACK: %d, HIST: %d\n", frnt, back, hist);
  for (i = frnt, h = 1; i != back; i = (i + 1) % BUFFER_SIZE, ++h) {
    printf("%d: %s\n", h, buffer[i]);
  }

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
    print_lines();
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
