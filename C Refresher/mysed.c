#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "support.h"


char* replace(char* search, char* replace, char* token){
  int tokindex = 0, match = 0, matchstart = 0;
  int searchlen = strlen(search), toklen = strlen(token);
  int replen;
  if(replace != NULL){
    replen = strlen(replace);
  }else{
    replen = 0;
  }
  int ccount = 1;
  char *cat = malloc(ccount*sizeof(char));
  cat[0] = 0;
  if(toklen == searchlen){
    return replace;
  }else{
    while(tokindex < toklen){
      if(search[0] == token[tokindex]){
	matchstart = tokindex;
	match++;
	tokindex++;
	while((tokindex < toklen) && (match > 0) && (match < searchlen)){
	  if((search[match]==token[tokindex]) && match < searchlen-1){
	    match++;
	    tokindex++;
	  }else if(match == (searchlen-1)){
	    ccount = ccount + replen + 1;
	    cat = realloc(cat, ccount*sizeof(char));
	    if(replace == NULL)
	      replace ="";
	    strcat(cat, replace);
	    match = 0;
	  }else{
	    char tmp1[2];
	    tmp1[0] = ' ';
	    tmp1[1] = '\0';
	    ccount = ccount + (tokindex - matchstart) + 1;
	    cat = realloc(cat, ccount*sizeof(char));
	    for(int i = matchstart; i < tokindex + 1; i++){
	      tmp1[0] = token[i];
	      strcat(cat, tmp1);
	    }
	    match = 0;
	    // matchstart = 0;
	  }
	}//end of match while
      }// end of initial match if
      else{
	char tmp2[2];
	tmp2[0] = ' ';
	tmp2[1] = '\0';
	ccount++;
	cat = realloc(cat, ccount*sizeof(char));
	tmp2[0] = token[tokindex];
	strcat(cat, tmp2);
      }
      tokindex++;
    }// end of outsied token while loop
    return cat;
  }// end of replace else
}// end of replace function

/*
 * sed_file() - modify lines of filename based on pattern, and print result
 *              to STDOUT
 */
void sed_file(char *filename, char *pattern) {
  printf("File Name: %s",filename);
  printf("\nPattern:   %s\n",pattern);
  FILE *fp = fopen(filename, "r");
  if(!fp){
    perror("file not found");
    exit(0);
  }

  char *s = "";
  char *searchstr = "";
  char *replacestr = "";
  char *g = "";
  const char slash[2] = "/";
  char *token;
  token = strtok(pattern, slash);
  int tokcount = 0;
  
  while(token != NULL){
    switch(tokcount){
    case 0: s = token; break;
    case 1: searchstr = token; break;
    case 2: replacestr = token; break;
    case 3: g = token; break;
    }
    token = strtok(NULL, slash);
    tokcount++;
  }
  if(tokcount == 3 && strcmp(s, "s") == 0 && strcmp(replacestr, "g") == 0){
    replacestr = 0;
    g = "g";
    tokcount++;
  }
  if((tokcount == 4) && (strcmp(s, "s") == 0) && (strcmp(g, "g") == 0)){
    char line[1024];
    char space[2] = " ";
    char *linetoken;
    char *ssret;
    
    while(fgets(line,1024, fp)){
      linetoken = strtok(line, space);
      while(linetoken != NULL){
	  if((ssret = strstr(linetoken, searchstr)) != NULL){	 
	    char *edit = replace(searchstr, replacestr, linetoken);
	    printf("%s ", edit);
	    free(edit);
     	  }
	  else{
	    if(linetoken[strlen(linetoken)-1] == '\n'){
	      printf("%s", linetoken);
	    }else{
	      printf("%s ", linetoken);
	    }
	  }
	linetoken = strtok(NULL, space);
      }
    }
  }else{
    printf("\n!Improper Format!\n");
    printf("Input must be of the form: \'s/search/replace/g\'\n");
  }
  
  fclose(fp);
}

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [FILE]\n", progname);
    printf("Print the contents of FILE to STDOUT\n");
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

    /* TODO: fix this invocation */
    sed_file(argv[argc-1], argv[optind]);
}
