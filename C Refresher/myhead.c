#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#define BUFFER 1024

/*
 * head_file() - display the first numlines lines of stdin
 */
void head_file(int numlines) {
  char line[BUFFER];
  int linecount = 0;
  while(fgets(line, BUFFER, stdin) != NULL && linecount < numlines){
    linecount++;
    printf(line);
  }
}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [OPTION]\n", progname);
    printf("Print the first 10 lines of STDIN\n");
    printf("  -nK    print the first K lines instead of the first 10\n");
}

/*
 * main() - The main routine parses arguments and dispatches to the
 *          task-specific code.
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;
    int numlines = 10; /* default number of lines */

    /* run a student name check */
    check_student(argv[0]);

    /* parse the command-line options.  For this program, we only support  */
    /* the parameterless 'h' option, for getting help on program usage, */
    /* and the parametered 'n' option, for getting the number of lines. */
    while ((opt = getopt(argc, argv, "hn:")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
          case 'n': numlines = atoi(optarg); break;
        }
    }

    /* no need for error checking here */
    head_file(numlines);
}
