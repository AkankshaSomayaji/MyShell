/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"
#include "../include/cmdtable.h"

/* ------------------------ Function Definitions ------------------------ */

void execute(int num)
{
		int i = num-1 , status;
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
