#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <dirent.h>

int str_compare(const void *,const void *);
/*
 * mylsr() - produce the appropriate directory listing(s)
 */
void mylsr(char **roots) {
    /* TODO: Complete this function */
  DIR *dir;
  DIR *nextdir;
  
  struct dirent *ent;
  int i = 1;
  // counter and directory name list declarations                                                                                                                                                                                          
  int namecount = 0;
  int numfiles = 0;
  char **name;
  char *tmpdir[1];
  
  // new variables
  char **directories;
  int dircount = 0;

  do{
    if(roots[i] == NULL){
      dir = opendir(".");
      printf("if(roots[i] == NULL){\n dir = opendir(\".\");\n}\n");
    } else{
      dir = opendir(roots[i]);
      printf("else{\n dir = opendir(\"%s\");\n}\n", roots[i]);
      // printf("%s:\n",roots[i]);
    }
    if(dir){
      if(roots[i] != NULL){
	printf("%s:\n",roots[i]);	
      }
      //      name = malloc(sizeof(char *)*50);
      name = (char **)malloc(sizeof(char**));
      while((ent = readdir(dir)) != NULL){
		printf("%s\n",ent->d_name);
	//	nextdir = opendir(ent->d_name);
	//	if(nextdir && ent->d_name[0] != '.'){
	  //	  tmpdir[1] = strdup(ent->d_name);
	  // mylsr(tmpdir);
	//	}
	//	closedir(nextdir);
	//	      tmpdir[1] = strdup(ent->d_name);
	//	      mylsr(tmpdir);
		     
		if(ent != NULL){
		name[namecount] = (char*)malloc(sizeof(ent->d_name));
		      name[namecount] = strdup(ent->d_name);
		      namecount++;
		}
	
      }
    }
    closedir(dir);
    //    int j;
    //    for(j = 0; i < namecount; j++){
    //      printf("%s\n", name[j]);
    //    }
    //    int k;
    //    for(k = 0; k < namecount; k++){
    //	free(name[k]);
    //    }
    free(name);


      
    /*
    if(dir){
      if(roots[i] != NULL)
      printf("%s:\n",roots[i]);
    name = malloc(sizeof(char *)*50);
    while((ent = readdir(dir)) != NULL)
    {
      if(ent)
	{
	  if(!('.'== (ent->d_name)[0])){
	      tmpdir[1] = strdup(ent->d_name);
	      mylsr(tmpdir);
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
      printf("\n");
    namecount =0; 
    if(roots[i] == NULL)
      break;
    else{
      i++;
    }
    }else{

    break; }*/
  }while(roots[i] != NULL);
    
}


int str_compare(const void *a, const void *b)
{
  const char **aa = (const char **)a;
  const char **bb = (const char **)b;
  return strcasecmp(*aa, *bb);
}
/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [FILE]...\n", progname);
    printf("List information about the FILEs (the current directory by default).\n");
    printf("List subdirectories recursively\n");
    printf("Behavior should mirror /bin/ls -1 -r\n");
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
    mylsr(argv);
}
