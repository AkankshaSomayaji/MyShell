/* --------------------------- Include Guards ---------------------------- */

#ifndef TOKENS_H
#define TOKENS_H

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

/*
command structure
*/

struct command_table
{
char ** cmdtkns;
int num_tokens;
char * outfile;
char * infile;
};

struct command_table cmd_table[25];
int no_cmd;

/* --------------------------- Function Headers -------------------------- */

/*
Tokenize the given input string and stores the tokens in 'tok' and the number of tokens in 'num_tok'
*/
void tokenize(char * input, char *** tok, int * num_tok);

/*
Prints the tokens generated for a given input string
*/
void print_tokens(char ** tokens, int num_tokens);
/*
prints command tabe
*/
//void print_cmdtable(struct command *cmd);
void make_cmdtable(char ** tokens, int num_tokens);
void print_cmdtable();
#endif
