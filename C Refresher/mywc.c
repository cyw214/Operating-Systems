#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "support.h"

/*
 * wc_file() - count characters, words, and lines
 */
void wc_file(char *filename) {
	// wc prints line, word count, character count, filename
	FILE *fp;
	fp = fopen(filename, "r");
	int character;
	int inword = 0;
	int linecount = 0;
	int wordcount = 0;
	int charactercount = 0;
	
	while( (character = getc(fp)) != EOF){
		if(inword == 0){
			if((character >= 65) && (character <= 90)){
				inword = 1;
				wordcount++;
			}else if((character >= 97) && (character <= 122)){
				inword = 1;
				wordcount++;
			}else if( (character >= 48) && (character <= 57)){
				inword = 1;
				wordcount++;
			}
		}// end of inword false
		
		if(character == ' ' || character == '\t'){
			inword = 0;
		}else if(character == '\n'){
			linecount++;
			inword = 0;
		}
		charactercount++;			
	}// end of while
	printf("%d %d %d %s\n",linecount, wordcount, charactercount, filename);
	fclose(fp);
}// end of wc_file

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s [FILE]\n", progname);
    printf("Print newline, word, and byte counts for FILE\n");
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

    /* error if we don't have a file, else call wc_file */
    if (argc != 2) {
        printf("Error: no file given\n");
        exit(1);
    }
    wc_file(argv[1]);
}
