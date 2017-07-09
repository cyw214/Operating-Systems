#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <dirent.h>

/*
 * myls() - produce the appropriate directory listing(s)
 */

int str_compare(const void *a, const void *b){
  const char *aa = (const char **)a;
  const char *bb = (const char **)b;
  return strcasecmp(*aa,*bb);
}

void remchar(char *input, char toremove){
  char *source, *output;
  for(source = output = input; *source != '\0'; source++){
    *output = *source;
    if(*output != toremove){
      output++;
    }// end if
  }// end for
  *output = '\0';
}

void myls(char **roots) {
  /* TODO: Complete this function */
  DIR *dir;
  struct dirent *ent;
  int i = 1;
  int namecount = 0;
  int numfiles = 0;
  char **name;
  
  do{
    if(roots[i] == NULL){
      dir = opendir(".");
    }else{
      dir = opendir(roots[i]);
      printf("%s:\n",roots[i]);
    }
    name = malloc(sizeof(char *)*50);
    while((ent = readdir(dir)) != NULL){
      if(ent){
	if(!('.' == (ent->d_name)[0])){
	  name[namecount] = (char *)malloc(sizeof(ent->d_name));
	  name[namecount] = strdup(ent->d_name);
	  namecount++;
	}// end inner if
      }// end outer if
      
    }
  
  }while(roots[i] != NULL);
}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [FILE]...\n", progname);
    printf("List information about the FILEs (the current directory by default).\n");
    printf("Behavior should mirror /bin/ls -1\n");
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
    /* TODO: make sure to handle any other arguments specified by the assignment */
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
        }
    }

    /* TODO: fix this invocation */
    myls(NULL);
}
