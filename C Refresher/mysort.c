#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "support.h"


int comparator(const void *str1, const void *str2){
  const char* s1 = *(const char**)str1;
  const char* s2 = *(const char**)str2; 
  return strcmp(s1,s2);
}

/*
 * sort_file() - read a file, sort its lines, and output them.  reverse and
 *               unique parameters are booleans to affect how the lines are
 *               sorted, and which lines are output.
 */
void sort_file(char *filename, int unique, int reverse) {
    /* TODO: Complete this function */
    /* Note: you will probably need to implement some other functions */
  char line[1024];
  FILE *fp = fopen(filename, "r");
  char **arr;
  int n = 1;
  fgets(line, 1024,fp);
  arr = malloc(n*sizeof(char *));
  arr[n-1] = malloc(sizeof(line));
  memmove(arr[n-1], line, strlen(line) + 1);

  while(fgets(line, 1024, fp) != NULL){  
    n++;
    arr = realloc(arr,n*sizeof(char *));
    arr[n-1] = (char *)malloc(sizeof(line));
    memmove(arr[n-1], line, strlen(line) + 1);
   }

  // sort lines
  qsort((void*)arr, n, sizeof(char*), comparator);
  // print sorted lines
  if(unique == 1){
  for(int i = 0; i < n; i++){
    for(int j = i+1; j < n; j++){
        if((strcmp(arr[i], arr[j]) == 0)){
	  memset(arr[j], '\0', strlen(arr[i])) ;
        }
    }
  }
  }

  if(reverse == 0)
    for(int i =0; i < n; i++){ printf(arr[i]);}
  else
    for(int i = n-1; i >= 0; i--){ printf(arr[i]);}

  for(int i =0; i < n; i++){
    free(arr[i]);
  }
  free(arr);
  fclose(fp);
}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [OPTIONS] FILE\n", progname);
    printf("Sort the lines of FILE and print them to STDOUT\n");
    printf("  -r    sort in reverse\n");
    printf("  -u    only print unique lines\n");
}

/*
 * main() - The main routine parses arguments and dispatches to the
 *          task-specific code.
 */
int main(int argc, char **argv) {
    /* for getopt */
    long opt;
    int unique = 0;
    int reverse = 0;
    /* ensure the student name is filled out */
    check_student(argv[0]);

    /* parse the command-line options.  They are 'r' for reversing the */
    /* output, and 'u' for only printing unique strings.  'h' is also */
    /* supported. */
    /* TODO: parse the arguments correctly */
    while ((opt = getopt(argc, argv, "hru")) != -1) {
        switch(opt) {
        case 'h': help(argv[0]); break;
	case 'r': reverse = 1; break;
	case 'u': unique = 1; break;
        }
    }

    /* TODO: fix this invocation */
    sort_file(argv[optind], unique, reverse);
}
