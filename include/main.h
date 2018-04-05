/* --------------------------- Include Guards ---------------------------- */

#ifndef MAIN_H
#define MAIN_H

/* ------------------------------ Includes ------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <termios.h> 
#include "tokens.h"
#include "execute.h"
#include "cmdtable.h"


struct alias{
	char **a_tok;
	char ** a_val;
	int tok_num;
	int val_num;
};
struct alias alias_table[10];
int alias_num=-1;
/* ------------------------------- Macros -------------------------------- */

/*
Define true as 1
*/
#define true 1

/*
Define false as 0
*/
#define false 0

/*
Maximum string length of the input commands
*/
#define MAX_INPUT 262144

/* --------------------------- Global Variables -------------------------- */

/*
Global environment variable
*/
extern char ** environ;

/* 
interger count of commands 
*/
extern int no_cmd;

/* --------------------------- Function Headers -------------------------- */

/*
Prints the greeting message
*/
void greet();

/*
Prints the command prompt
*/
void prompt();

/*
Performs all clean up actions
*/
void clean_up(char * input, char ** tokens, int num_tokens);
//---------------
void set_alias_table(int num_tokens,char ** tokens);
int check(char * tok);
#endif
