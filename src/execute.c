/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"

/* ------------------------ Function Definitions ------------------------ */

void execute(int num)
{
		int i = num-1, j , status;
		pid_t pid;

		char **arg = (char **)malloc(sizeof(char *) * 1);
		for(j = 0; j < cmd_table[i].num_tokens ; j++)
		{
			arg = (char **)realloc(arg,sizeof(char *) * (j+1));
			arg[j] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
			strcpy(arg[j],cmd_table[i].cmdtkns[j]);	
		}
		arg = (char **)realloc(arg,sizeof(char *) * (j));
		arg[j] = (char *)0;

		
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
