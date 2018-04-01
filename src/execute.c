/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"
#include "../include/cmdtable.h"

/* ------------------------ Function Definitions ------------------------ */

void execute(int num){

		int i = num - 1 , status , fd = -1;
		pid_t pid;
		int ret = 0;

		cmd_table[i].cmdtkns = (char **)realloc(cmd_table[i].cmdtkns,sizeof(char *)*(cmd_table[i].num_tokens+1));
		cmd_table[i].cmdtkns[cmd_table[i].num_tokens] = NULL;

		if(strcmp(cmd_table[i].cmdtkns[0], "cd") == 0){
			ret = change_directory(cmd_table[i].cmdtkns);
			if(ret < 0){
				perror("cd");
			}
			else{
				return;
			}
		}
		
		if((pid=fork())<0){
			perror("fork");
		}
		else if(pid==0){
			if(strcmp("stdin",cmd_table[i].infile)!=0){
				fd = open(cmd_table[i].infile,O_RDONLY,0644);
				close(0);
				dup(fd);
				
			}
			if(strcmp("stdout",cmd_table[i].outfile)!=0){
				if(cmd_table[i].out_append == 1){
					fd = open(cmd_table[i].outfile,O_CREAT|O_APPEND|O_WRONLY,0644);
				}
				else{
					fd = open(cmd_table[i].outfile,O_CREAT|O_WRONLY,0644);
				}
				close(1);
				dup(fd);
				
			}

			if(strcmp("stdout",cmd_table[i].errfile)!=0){
				close(1);
				fd = open(cmd_table[i].errfile,O_CREAT|O_WRONLY,0644);
				close(2);
				dup(fd);
			}	

			if(execvp(cmd_table[i].cmdtkns[0],cmd_table[i].cmdtkns)<0){
				perror("exec");
				printf("Command doesnt exist\n");
				exit(1);
			}
		}
		else if(pid>0){
			pid = waitpid(pid,&status,0);		
		}
}

int change_directory(char ** arg){
    int ret = 0;

	if (arg[1] == NULL){
		chdir(getenv("HOME")); 
		return 0;
	}

	else{ 
        ret = chdir(arg[1]); 
		if(ret < 0){
            return -1;
		}
	}
	return 0;
}
