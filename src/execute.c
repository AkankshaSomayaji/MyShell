/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"

/* ------------------------ Function Definitions ------------------------ */

void execute(int num)
{
		int i = num-1, j , status;
		pid_t pid;
		int ret = 0;

		char **arg = (char **)malloc(sizeof(char *) * 1);
		for(j = 0; j < cmd_table[i].num_tokens ; j++)
		{
			arg = (char **)realloc(arg,sizeof(char *) * (j+1));
			arg[j] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
			strcpy(arg[j],cmd_table[i].cmdtkns[j]);	
		}
		arg = (char **)realloc(arg,sizeof(char *) * (j+1));
		arg[j] = NULL;

		if(strcmp(arg[0], "cd") == 0){
			ret = change_directory(arg);
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
			if(execvp(arg[0],arg)<0){
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
