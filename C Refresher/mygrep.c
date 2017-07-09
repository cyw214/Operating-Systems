#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"

/*
 * grep_file() - display all lines of filename that contain searchstr
 */
void grep_file(char *filename, char *searchstr) {
    /* TODO: Complete this function */
  int searchlen = strlen(searchstr);
  FILE *fp = fopen(filename, "r");
  char line[1024];
  int linelen = 0;
  int linecount = 0;
  int found = 0;
  int foundcount = 0;
  
  while(fgets(line, 1024, fp) != NULL){
    linelen = strlen(line);
    linecount = 0;
    foundcount = 0;
    found = 0;
    while((linecount < linelen) && (found == 0)){
      if(line[linecount] == searchstr[foundcount]){
	if(foundcount == (searchlen-1) ){
	  printf(line);
	  found = 1;
	}
	linecount++;
	foundcount++;	
      }else{
	foundcount = 0;
	linecount++;
      }
     }
    }
  }
	
  


/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s STR FILE\n", progname);
    printf("Print to STDOUT the lines of FILE that contain STR\n");
    printf("With no FILE, read standard input\n");
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

    /* make sure we have two more parameters */
    if (optind != argc - 2) {
        printf("Error: not enough arguments\n");
        exit(1);
    }

    /* TODO: fix this invocation */
    grep_file(argv[2], argv[optind]);
}
