/* --------------------------- Include Guards ---------------------------- */

#ifndef CMDTABLE_H
#define CMDTABLE_H

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

/* --------------------------- Global Variables -------------------------- */

/*
global command structure
*/
struct command_table{
    char ** cmdtkns;
    int num_tokens;
    char * outfile;
    char * infile;
};

/* 
global command table 
*/
extern struct command_table cmd_table[25];

/* 
interger count of commands 
*/
extern int no_cmd;

/* --------------------------- Function Headers -------------------------- */

/*
construct command table
*/
void make_cmdtable(char ** tokens, int num_tokens);

/*
print command table
*/
void print_cmdtable();

/*
Shift command table by 1 after 25 commands
*/
void shift_table();

/*
Writes 25 most recent commands to the history file
*/
void update_history();

#endif