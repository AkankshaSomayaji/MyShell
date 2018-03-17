#ifndef MAIN_H
#define MAIN_H

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

/*
Global environment variable
*/
extern char ** environ;

/*
Prints the greeting message
*/
void greet();

/*
Prints the command prompt
*/
void prompt();

#endif