#define _GNU_SOURCE
#include <getopt.h>
#include <stdio.h>
#include <netdb.h>
#include "support.h"
#include <unistd.h>
#include <string.h>
/*
 * host() - Print the hostname of the machine on which this program is
 *          running, formatted a few different ways
 */
void host(void) {
  char name[64];
  char uc = ' ';
  char mc = ' ';
  gethostname(name, sizeof(name));
  printf("%s\n", name);
  for(int i = 0; i < strlen(name); i++){
    uc = toupper(name[i]);
    printf("%c", uc);
  }
  printf("\n");
  for(int i = 0; i< strlen(name); i ++){
    if(i %2 == 0){
      mc = tolower(name[i]);
    }else{
      mc = toupper(name[i]);
    }
    printf("%c", mc);
  }
  printf("\n");
  for(int i = strlen(name)-1; i >= 0; i--){
    printf("%c", name[i]);
  }
  printf("\n");
  


}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s\n", progname);
    printf("Display the hostname of the machine\n");
    printf("The name will be displayed four ways, in the following order, on "
           "four lines:\n");
    printf("  all lowercase\n");
    printf("  ALL UPPERCASE\n");
    printf("  mIxEd LoWeRaNdUpPeR\n");
    printf("  emantsoh (i.e., reversed)\n");
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

    /* no need to look at arguments before calling the code */
    host();
}
