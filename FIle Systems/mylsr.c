#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
/*
 * mylsr() - produce the appropriate directory listing(s)
 */
/*int leadingzero(char *a, char *b, int in1, int in2){
  int alen = strlen(a);
  int blen = strlen(b);
  if
}
*/
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

void mylsr(char **);



void list(const char *dirpath){
  struct dirent *content;
  DIR *dir;
  dir = opendir(dirpath);
  if(dir != NULL){
    char **name;
    int n = 0;
    name = malloc(sizeof(char *));
    while(content = readdir(dir)){
      if(content->d_name[0] != '.'){
	n++;
	if(n == 1){
	  //	  name = (char *)malloc(strlen(n*sizeof(char *)));
	}else{
	  name = realloc(name,n*sizeof(char *));
	}
	name[n-1] = malloc(sizeof(char *)*512);
	memmove(name[n-1], content->d_name, strlen(content->d_name) + 1);
      } 
    }

   
      qsort((void*)name,n,sizeof(char *), str_compare);
      int i =0;
      for(i =0; i < n; i++){
	printf("%s\n",name[i]);
      }
    i = 0;
      
    while(i<n){
      //struct stat st;
      //if(lstat(name[i], &st) == 0 && S_ISDIR(st.st_mode)){
      //      char subpath[strlen(name[i])+strlen(dirpath)];
      char postfix[512];
      char prefix[512];
      char slash[2];
      strcpy(postfix, name[i]);
      strcpy(prefix, dirpath);
      strcpy(slash, "/");
      strcat(prefix, postfix);
      strcat(prefix, slash);
      char **subdir = malloc(sizeof(char*));
      subdir[0] = malloc(sizeof(prefix));
      memmove(subdir[0], prefix, strlen(prefix) + 1);
      subdir = realloc(subdir,2*sizeof(char *));
      subdir[1] = malloc(sizeof(prefix));
      memmove(subdir[1], prefix, strlen(prefix) + 1);
      //      strcpy(subdir[1], dirpath);
      //      strcpy(subname, name[i]);
      //      strcat(subname, "/");
      //      strcat(subdir[1],subname);
      struct stat st;
      if(lstat(subdir[1], &st) == 0 && S_ISDIR(st.st_mode))
	mylsr(subdir);
      free(subdir[0]); free(subdir[1]); free(subdir);
      free(name[i]);
      i++;
    }
   
   free(name);
    
  }
  closedir(dir);
}

static int firstarg = 1;

void mylsr(char **roots) {
  /* TODO: Complete this function */
  int i = 1;
  char *path = (char *)malloc(sizeof(char*)*512);
  int rootflag = 0;
  if(roots[i] == NULL){
    path[0] = '\0';
    strcat(path, "./");
    rootflag = 0;
    printf(".:\n");
    firstarg = 0;
  }else{
    path[0] = '\0';
    strcat(path, roots[i]);
    if(firstarg == 0)
      printf("\n");
    if(path[strlen(path)-1] == '/'){
      char pcopy[strlen(path)];
      memmove(pcopy, path, strlen(path) + 1);
      pcopy[strlen(path)-1] = '\0';
      printf("%s:\n",pcopy);
    }
    if(path[strlen(path)-1] != '/'){
      printf("%s:\n",path);
      strcat(path, "/");
    }
    firstarg = 0;
  }
  //  struct stat st;
  //if(lstat(path, &st) == 0 && S_ISDIR(st.st_mode))
    list(path);
  free(path);
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
