/**
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

#define BUFFER_SIZE 11

// Function prototypes
void signal_handler(int);
clock_t program_time();
double ptime();
void add_line(const char*);
void print_lines();

// Globals
char* buffer[BUFFER_SIZE];
int frnt = 0, back = 0, hist = 0;
pthread_mutex_t buffer_lock;
struct sigaction act;


int main(void)
{
  // Setup mutex
  if (pthread_mutex_init(&buffer_lock, NULL) != 0) {
    fprintf(stderr, "Could not create mutex...exiting\n");
    exit(1);
  }

  // Setup signal handler
  sigemptyset(&act.sa_mask);
  sigaddset(&act.sa_mask, SIGTSTP);
  sigaddset(&act.sa_mask, SIGALRM);
  act.sa_handler = signal_handler;
  act.sa_flags = SA_RESTART;
  sigaction(SIGALRM, &act, NULL);
  sigaction(SIGINT,  &act, NULL);
  sigaction(SIGTERM, &act, NULL);
  sigaction(SIGTSTP, &act, NULL);

  // Setup alarm
  struct itimerval timer;
  timer.it_interval.tv_sec = 10;
  timer.it_interval.tv_usec = 1;
  timer.it_value = timer.it_interval;
  setitimer(ITIMER_REAL, &timer, NULL);

  // Capture user input and place into ring buffer
  while(1) {
    char* line = NULL;
    size_t len = 0;
    ssize_t linelen;

    // If input, invoke mutex to lock shared resource to prevent race
    // condition. Then add the new input into the ring buffer and
    // adjust indices into the ring buffer. Finally, release lock to
    // others can access it.
    linelen = getline(&line, &len, stdin);
    fflush(stdin);
    if (linelen >= 0) printf("LEN: %d\n",linelen);
    if (linelen != -1) {
      fprintf(stderr,"got it\n");
      pthread_mutex_lock(&buffer_lock);
      line[linelen-1] = '\0';
      if (back < frnt)
        free(buffer[back]);
      buffer[back] = line;
      back = (back + 1) % BUFFER_SIZE;
      if (back == frnt)
        frnt = (frnt + 1) % BUFFER_SIZE;
      ++hist;
      pthread_mutex_unlock(&buffer_lock);
    }
  }
}

/**
 * Signal handler which can deal with SIGALRM, SIGINT, SIGTSTP, and
 * SIGTERM.
 */
void signal_handler(int status)
{
  // Local variable which persists between invocations of the handler.
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
    printf("Program time: %f\nCleaning up...\n", ptime());
    for (; frnt != back; frnt = (frnt + 1) % BUFFER_SIZE)
      free(buffer[frnt]);
    exit(0);
  }
}

/**
 * Print the contents of the ring buffer along with their history
 * number.
 */
void print_lines()
{
  int i, h;
  pthread_mutex_lock(&buffer_lock);
  for (i = frnt; i != back; i = (i + 1) % BUFFER_SIZE, ++h) {
    printf("  %s\n", buffer[i]);
  }
  fflush(stdout);
  pthread_mutex_unlock(&buffer_lock);
}

/**
 * Calculate time spent in user and system poritions of the process.
 */
double ptime()
{
  static struct rusage usage;
  getrusage(RUSAGE_SELF, &usage);
  double time = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
  time += (usage.ru_utime.tv_usec / 1000.0) + (usage.ru_stime.tv_usec / 1000.0);
  return time;
}
