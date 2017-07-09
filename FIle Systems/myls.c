#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <dirent.h>
#include <ctype.h>

int str_compare(const void *,const void *);

/*
 * myls() - produce the appropriate directory listing(s)
 */

void myls(char **roots) {
    /* TODO: Complete this function */
    
  DIR *dir;
  struct dirent *ent;
  int i = 1;
  // counter and directory name list declarations
  int namecount = 0;
  int numfiles = 0;
  char **name;
  
  do{
    if(roots[i] == NULL){
      dir = opendir(".");
    } else{
      dir = opendir(roots[i]);
      
    }
    // malloc space for the list of directory names
    name = malloc(sizeof(char *)*50);
      while((ent = readdir(dir)) != NULL)
      {
        if(ent)
        {
	  if(!('.'== (ent->d_name)[0])){
	    name[namecount] = (char*)malloc(sizeof(ent->d_name));
	    name[namecount] = strdup(ent->d_name);
	    namecount++;
	  }
        }     
      }
      closedir(dir);
      numfiles = namecount;
      namecount = 0;
      qsort(name, numfiles, sizeof(char *), *str_compare);
      while(name[namecount] != NULL)
        {
          printf("%s\n", name[namecount]);                                                                     
          free(name[namecount]);
          namecount++;
        }
      free(name);
      namecount =0;
      if(roots[i] == NULL)
	break;
      else{
	i++;
      }
     }while(roots[i] != NULL);
}

// returns 1 if valid 0 if invalid
int valid(char c){
  if((c > 64 && c < 91) || (c > 96 && c < 123) || (c > 48 && c < 57))
    return 1;
  else
    return 0;
}

int str_compare(const void *a, const void  *b)
{
  const char **aa = (const char **)a;
  const char **bb = (const char **)b;
  int aalen = strlen(*aa); int bblen = strlen(*bb);
  char acopy[aalen]; char bcopy[bblen];
  strcpy(acopy, *aa); strcpy(bcopy, *bb); 
  int i = 0; int j = 0;
  while((i < bblen) && (j < aalen)){
    char ac = tolower(acopy[i]); char bc = tolower(bcopy[j]);
    if(valid(ac) && valid(bc)){
      if(ac > bc)
	return 1;
      else if(ac < bc)
	return -1;
      i++; j++;
    }else{
      if(!valid(ac))
	i++;
      else
	j++;   
    }
  }  
  if(aalen == bblen)
    return 0;
  else if(aalen > bblen)
    return 1;
  else if(aalen < bblen)
    return -1;
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
    myls(argv);
}
