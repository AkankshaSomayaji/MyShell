#ifndef TOKENS_H
#define TOKENS_H

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
Tokenize the given input string and stores the tokens in 'tok' and the number of tokens in 'num_tok'
*/
void tokenize(char * input, char *** tok, int * num_tok);

/*
Prints the tokens generated for a given input string
*/
void print_tokens(char ** tokens, int num_tokens);

#endif