#include <string.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include "support.h"
#include <ctype.h>
#define true   1;
#define false  0;

typedef struct Node{
  int  data;
  struct Node *next;
}Node;

Node * head = NULL;

void printList(){
  Node *curr = head;
  while(curr != NULL){
    if(curr->next != NULL){
      printf("%d->",curr->data);
    }else{
      printf("%d", curr->data);
    }
    curr = curr->next;
  }
  printf("\n");
}

void swap(Node * node1, Node * node2){
    int temp;
    temp = node1->data;
    node1->data = node2->data;
    node2->data = temp;
}

void insert(int val){
  if(head == NULL){
    head = (Node *)malloc(sizeof(Node));
    head->data = val;
    head->next = NULL;
   }else{
    Node * new = malloc(sizeof(Node));
    new->data = val;
    new->next = head;
    head = new;
    Node * curr = head;
    while(curr != NULL){
      if(curr->next != NULL){
	if(curr->data > curr->next->data)
	  swap(curr->next, curr);
      }
      curr = curr->next;
    }
  }
  return;
}

void delete(int val){
  Node * prev = NULL;
  Node * curr = head;
  Node * rm = NULL;
  if(head == NULL){
    return;
  }

  if(curr->next == NULL){
    if(curr->data == val){
      rm = head;
      free(head);
      head = NULL;
    }
    return;
  }else{
    if(curr->data == val){
      Node *newHead = head->next;
      free(head);
      head = newHead;
    }else{
      while(curr->next != NULL){
        prev = curr;
	curr = curr->next;
	if(curr->data == val){
	  break;
	}
      }// end while
      if(curr->data == val){
	rm = curr;
	prev->next = curr->next;
	free(rm);
      }
      return;      
    }   
  }// end main if else
}

void destroy(){
  Node *curr = head;
  Node *fnode = NULL;
  if(curr != NULL){
    while(curr->next != NULL){
      fnode = curr;
      curr = curr->next;
      free(fnode);
    }
    free(curr);
    head = NULL;
  }  
}

int getNum(char * str, int * num){
  char numstr[1024] = "";
  if((str[0] == 'i' || str[0] == 'r') && (str[1] == ' ')){
    for(int i = 2; i < strlen(str)-1; i++ ){
      if(isdigit(str[i]) || str[i] == '-'){
	numstr[i-2] = str[i];
      }else{ printf("Integer Arguments Only\n"); return 0;}
    }
    *num = atoi(numstr);
    return 1;
  }else{
    printf("Invalid Input\n");
      return 0;
  }
  
}

void I(int val){
  insert(val);
}

void R(int val){
  delete(val);
}

void X(){
  destroy();
  exit(0);
}

void P(){
  printList();
}

/*
 * list_task() - read from stdin, determine the appropriate behavior, and
 *               perform it.
 */
void list_task(void) {
  char line[1024] = "";
  int flag = 0;
  int command = ' ';
  while(fgets(line,1024,stdin) != NULL){
    command = line[0];
    if((command == 'i') && (getNum(line, &flag) == 1)){
      I(flag);
    }else if(command == 'r' && (getNum(line, &flag) == 1)){
      R(flag);
    }else if(command == 'p'){
      P();
    }else if(command == 'x'){
      X();
    }else{ printf("Invalid Input\n"); }
  }// end while

  
}// end list_task

/*
 * help() - Print a help message.
 */
void help(char *progname) {
    printf("Usage: %s\n", progname);
    printf("Run an interactive list manipulation program\n");
    printf("Commands: [i]insert, [r]emove, [p]rint, e[x]it\n");
    printf("  (Commands require an integer argument)\n");
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

    /*
     * parse the command-line options.  For this program, we only support the
     * parameterless 'h' option, for getting help on program usage.
     */
    while ((opt = getopt(argc, argv, "h")) != -1) {
        switch(opt) {
          case 'h': help(argv[0]); break;
        }
    }

    /* run the list program */
    list_task();
}
