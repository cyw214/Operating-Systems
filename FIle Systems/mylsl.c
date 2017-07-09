#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <locale.h>
#include <langinfo.h>
#include <stdint.h>
#include <math.h>

int str_compare(const void *,const void *);
/*
 * mylsl() - produce the appropriate directory listing(s)
 */

static int argnum;
void mylsl(char **roots) {
    /* TODO: Complete this function */
  DIR *dir;
  struct dirent *ent;
  int i = 1;
  int j;
  int namecount = 0;
  int numfiles = 0;
  char **name;
  char *pathbuffer;
  char *statbuffer;
  struct stat fstat;
  struct passwd *pwd; 
  struct group *grp;
  struct tm *tm;
  char dates[256];
 
  do{

    pathbuffer = (char *)malloc(sizeof(char *)*256);
    statbuffer = (char *)malloc(sizeof(char *)*256);
    if(roots[i] == NULL){
      dir = opendir(".");
      pathbuffer[0] = '.';
      pathbuffer[1] = '/';
      if(argnum > 2){
	if((i > 2) || (i > 1))
          printf("\n%s:\n", pathbuffer);
          //printf("%s:\n", pathbuffer);
	  //else

      }
    } else{
      dir = opendir(roots[i]);
      pathbuffer[0] = '\0';
      strcat(pathbuffer,roots[i]);
      if(argnum >= 2){
	if(i > 1)
	  printf("\n%s:\n", pathbuffer);
	else
	  printf("%s:\n", pathbuffer);
      }
      strcat(pathbuffer, "/");
    }
    name = malloc(sizeof(char *)*100);
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
    
    unsigned int blockcount = 0;
    int k = 0;
    while(k < numfiles){
      statbuffer = (char *)malloc(sizeof(char *)*256);
      statbuffer[0] = '\0';
      strcat(statbuffer,pathbuffer);
      strcat(statbuffer, name[k]);
      stat(statbuffer, &fstat);
      blockcount = blockcount + fstat.st_blocks;
      free(statbuffer); 
      k++;
    }
    float total = ceil(((float)blockcount)/2.0);
    printf("total %d\n", (int)total);
    blockcount = 0;
    k = 0;
    while(k < numfiles)
    {
      statbuffer = (char *)malloc(sizeof(char *)*256);
      statbuffer[0] = '\0';
      strcat(statbuffer,pathbuffer);
      strcat(statbuffer, name[k]);
      stat(statbuffer, &fstat);
      printf((S_ISDIR(fstat.st_mode))  ? "d" : "-");
      printf((fstat.st_mode & S_IRUSR) ? "r" : "-");
      printf((fstat.st_mode & S_IWUSR) ? "w" : "-");
      printf((fstat.st_mode & S_IXUSR) ? "x" : "-");
      printf((fstat.st_mode & S_IRGRP) ? "r" : "-");
      printf((fstat.st_mode & S_IWGRP) ? "w" : "-");
      printf((fstat.st_mode & S_IXGRP) ? "x" : "-");
      printf((fstat.st_mode & S_IROTH) ? "r" : "-");
      printf((fstat.st_mode & S_IWOTH) ? "w" : "-");
      printf((fstat.st_mode & S_IXOTH) ? "x" : "-");
      printf(" ");
      printf("%d ", fstat.st_nlink);
      pwd = getpwuid(fstat.st_uid);
      printf("%s ", pwd->pw_name);
      grp = getgrgid(fstat.st_gid);
      printf("%s ", grp->gr_name);
      printf("%5jd ", (intmax_t)fstat.st_size);
      tm = localtime(&fstat.st_mtime);
      strftime(dates, sizeof(dates), "%b %d %H:%M", tm);
      printf("%s ", dates);
      printf("%s \n", name[namecount]);
      free(name[namecount]);
      namecount++;
      free(statbuffer);
      k++;
    }
    free(pathbuffer);
    //free(name);
    namecount =0;
    if(roots[i] == NULL)
      break;
    else{
      i++;
    }
  }while(roots[i] != NULL);
}

int valid(char c){
  if((c > 64 && c < 91) || (c > 96 && c < 123) || (c > 47 && c < 58))
    return 1;
  else
    return 0;
}

int str_compare(const void *a, const void *b)
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
    printf("Use a long listing format\n");
    printf("Behavior should mirror /bin/ls -1 -l\n");
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
    argnum = argc;
    mylsl(argv);
}
