#include <getopt.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>

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

void greplist(char *, const char*);
/*
 * myrgrep() - find files (recursively) with matching pattern
 */
void myrgrep(char *pattern, char **roots) {
  /* TODO: Complete this function */
  int i = 2;
  char *path = (char *)malloc(sizeof(char*)*512);
  if(roots[2] == NULL){
    path[0] = '\0';
    strcat(path, "./");
  }else{
    path[0] = '\0';
    strcat(path, roots[i]);
    if(path[strlen(path)-1] != '/'){
      strcat(path, "/");
    }
  }
  greplist(pattern, path);
  free(path);
}

// returns 1 if found and 0 otherwise
int grep_file(char *filename, char *searchstr) {
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
          fclose(fp);
	  return 1;
        }
        linecount++;
        foundcount++;
      }else{
        foundcount = 0;
        linecount++;
      }
    }
  }
  fclose(fp);
  return 0;
}

void greplist(char *pattern, const char *dirpath){
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
      char prepath[512]; char postpath[512]; char slash[2];
      strcpy(prepath, dirpath); strcpy(postpath, name[i]);
      strcpy(slash,"/");
      if(dirpath[strlen(dirpath)-1] != '/')
	strcat(prepath, slash);
      strcat(prepath, postpath);
      char *completepath = malloc(sizeof(prepath));
      memmove(completepath, prepath,strlen(prepath)+1);
      struct stat fst;
      if(lstat(completepath, &fst) == 0 && S_ISREG(fst.st_mode))
      {
	if(grep_file(completepath, pattern) == 1){
	    printf("%s\n",completepath);
	}
      }
      free(completepath);
    }
    i = 0;
    while(i<n){
      char postfix[512]; char prefix[512]; char slash[2];
      strcpy(postfix, name[i]);
      strcpy(prefix, dirpath);
      strcpy(slash, "/");
      strcat(prefix, postfix);
      strcat(prefix, slash);
      char **subdir = malloc(sizeof(char*));
      subdir[0] = malloc(sizeof(slash));
      memmove(subdir[0], slash, strlen(slash) + 1);
      subdir = realloc(subdir,2*sizeof(char *));
      subdir[1] = malloc(sizeof(slash));
      memmove(subdir[1], slash, strlen(slash) + 1);
      subdir = realloc(subdir,3*sizeof(char *));
      subdir[2] = malloc(sizeof(prefix));
      memmove(subdir[2], prefix, strlen(prefix) + 1);
      struct stat st;
      if(lstat(subdir[2], &st) == 0 && S_ISDIR(st.st_mode))
        myrgrep(pattern,subdir);
      free(subdir[0]); free(subdir[1]); free(subdir[2]); free(subdir);
      free(name[i]);
      i++;
    }
    free(name);
  }
  closedir(dir);
}
/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [OPTIONS] [PATTERN] [FILE]...\n", progname);
    printf("List the names of the FILEs (the current directory by default)\n");
    printf("whose contents contain PATTERN.\n");
    printf("The search should be recursive into subdirectories of any FILE\n");
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
    /* TODO: make sure to handle any other arguments specified by the */
    /*       assignment */
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
        }
    }

    /* TODO: fix this invocation */
    myrgrep(argv[optind], argv);
}
