#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <unistd.h>
/*
 * tail_file() - display the last numlines lines of a file or stdin
 */
void tail_file(char *filename, int numlines) {
      FILE *fp;
      if(filename == NULL){
	fp = stdin;
      }else{
	if(access(filename, R_OK) != -1){
	  fp = fopen(filename, "r");
	}
	else{
	  perror("Error: ");
	  exit(0);
	}
      }
      char line[1024];
      char tail[numlines][1024];
      int ringIndex = 0;
      int curr = 0;
      int start = 0;
      int end = 0;
      int linecount = 0;
      if(numlines > 0){
	while(fgets(line, 1024, fp) != NULL && atoi(line) != EOF){
	  if(atoi(line) != EOF && line != NULL){
	    linecount++;
	    if(curr == numlines - 1){
	      ringIndex = numlines - 1;
	      curr = 0;
	    }else{
	      ringIndex = curr;
	      curr++;
	    }
	strcpy(tail[ringIndex], line);
	end = curr;
	if(end == start && start < numlines - 1){
	  start++;
	}else if (end == start && start == numlines - 1){
	  start = 0;
	}
	}else {break;}
      }
     int lastindex;
     if(linecount < numlines)
	lastindex = linecount;
     else
	lastindex = numlines;
      
      for(int i= start-1 ; i < lastindex; i++ ){
      	printf(tail[i]);
      }
      if(end < start){
	for(int j = 0; j < start-1; j++){
      	  printf(tail[j]);
      	}
      }
      }else{printf(" ");}

	fclose(fp);

}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [FILE]\n", progname);
    printf("Print the last 10 lines  of FILE to STDOUT\n");
    printf("With no FILE, read from STDIN\n");
    printf("  -nK    print the last K lines instead of the last 10\n");
}

/*
 * main() - The main routine parses arguments and dispatches to the
 *          task-specific code.
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;
    int numlines = 10;
    extern char *optarg;
    extern int optind;
    /* run a student name check */
    check_student(argv[0]);
    
    /* parse the command-line options.  For this program, we only support  */
    /* the parameterless 'h' option, for getting help on program usage, */
    /* and the parametered 'n' option, for getting the number of lines. */
    /* TODO: parse the arguments correctly */
    while ((opt = getopt(argc, argv, "hn::")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
	  case 'n': numlines =  atoi(optarg);
	    if(optarg != NULL){
	      numlines = atoi(optarg);
	    }else{
	      numlines = 10;
	    }
	    break;
        }
    }
    /* TODO: fix this invocation */
   
      tail_file(optind < argc ? argv[optind] : NULL, numlines); 
   
   
}
