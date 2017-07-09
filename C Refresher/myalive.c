#include <getopt.h>
#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "support.h"
#include <unistd.h>

volatile sig_atomic_t loop = 1;
volatile sig_atomic_t print = 0;

void Alarmhandle(int signum){
  loop = 0;
  signal(signum, Alarmhandle);
}

void INThandle(int signum){
  if(signum == SIGINT){
    print = 1;
  }
  signal(signum, INThandle);
}

/*
 * alive() - install some signal handlers, set an alarm, and wait...
 */
void alive(void) {
    /* TODO: Complete this function */
    /* Note: you will probably need to implement some other functions */
  signal(SIGINT, INThandle);
  signal(SIGALRM, Alarmhandle);
  alarm(10);
  while(loop){
    if(print == 1){
      printf("NO\n");
      print = 0;
    }
  }
}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s\n", progname);
    printf("Swallow SIGINT, exit on SIGALARM\n");
}

/*
 * main() - The main routine parses arguments and dispatches to the
 *          task-specific code.
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;

    /* run a student name check */
    check_student(argv[0]);

    /* parse the command-line options.  For this program, we only support  */
    /* the parameterless 'h' option, for getting help on program usage. */
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
        }
    }
    struct timeval tvStart;
    struct timeval tvEnd;
    time_t StartTime;
    time_t EndTime;
    
    /* no need to look at arguments before calling the code */
    gettimeofday(&tvStart, NULL);
    StartTime = tvStart.tv_sec;
    alive();
    gettimeofday(&tvEnd, NULL);
    EndTime = tvEnd.tv_sec;
    time_t ElapsedTime = EndTime - StartTime;
    printf("ElapsedTime: %ld sec\n", (long)ElapsedTime);
}
