/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"
#include "../include/cmdtable.h"

/* ------------------------ Function Definitions ------------------------ */

void execute(int num){

		int i = num - 1;

		cmd_table[i].cmdtkns = (char **)realloc(cmd_table[i].cmdtkns,sizeof(char *)*(cmd_table[i].num_tokens+1));
		cmd_table[i].cmdtkns[cmd_table[i].num_tokens] = NULL;

		if(strcmp(cmd_table[i].cmdtkns[0], "cd") == 0){
			int ret = 0;
			ret = change_directory(cmd_table[i].cmdtkns);
			if(ret < 0){
				perror("cd");
			}
			else{
				return;
			}
		}
		if(cmd_table[i].pipe != 0)
		{
			execute_pipe_command(num);
		}
		else{
			execute_simple_command(num);
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

void execute_simple_command(int num)
{
	int i = num - 1 , status ;
	pid_t pid;
	if((pid=fork())<0){
		perror("fork");
	}
	else if(pid==0){
		int fd = -1;
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
				fd = open(cmd_table[i].outfile,O_CREAT|O_WRONLY|O_TRUNC,0644);
			}
			close(1);
			dup(fd);				
		}

		if(strcmp("stdout",cmd_table[i].errfile)!=0){
			close(1);
			fd = open(cmd_table[i].errfile,O_CREAT|O_WRONLY|O_TRUNC,0644);
			close(2);
			dup(fd);
		}
		if(execvp(cmd_table[i].cmdtkns[0],cmd_table[i].cmdtkns)<0){
			perror("exec");
			exit(1);
		}
	}
	else if(pid>0){
		pid = waitpid(pid,&status,0);		
	}
}

void execute_pipe_command(int num)
{
	char ** pipe_cmds = malloc(sizeof(char *) * 1);
	int k = 0, j = 0 , set = 0 , last = 0;
	int tmpin = dup(0);
	int tmpout = dup(1);

	int i = num - 1 , status;
	pid_t pid;


	int fdin;
	if(strcmp("stdin",cmd_table[i].infile)!=0){
		fdin = open(cmd_table[i].infile,O_RDONLY,0644);
	}
	else{
		fdin = dup(tmpin);
	}
	int fdout;
	for(j = 0; j < cmd_table[i].num_tokens; j++)
	{
		set = 0;
		if(cmd_table[i].pipe == 0)
		{
			k = 0;
			pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *)*1);
			int t;
			for(t = j; t < cmd_table[i].num_tokens; t++){
				pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *) * (t + 1));
				pipe_cmds[k] = (char *)malloc(sizeof(char)*(strlen(cmd_table[i].cmdtkns[t])+ 1));
				strcpy(pipe_cmds[k],cmd_table[i].cmdtkns[t]);
				k = k + 1;
			}
			set = 1;	
			last = 1;
			if(strcmp("stdout",cmd_table[i].outfile)!=0){
				if(cmd_table[i].out_append == 1){
					fdout = open(cmd_table[i].outfile,O_CREAT|O_APPEND|O_WRONLY,0644);
				}
				else{
					fdout = open(cmd_table[i].outfile,O_CREAT|O_WRONLY|O_TRUNC,0644);
				}
			}
			else{
				fdout = dup(tmpout);
			}
			if(strcmp("stdout",cmd_table[i].errfile)!=0){
				int fd;
				close(1);
				fd = open(cmd_table[i].errfile,O_CREAT|O_WRONLY|O_TRUNC,0644);
				close(2);
				dup(fd);
			}
		}		
		if(strcmp("|",cmd_table[i].cmdtkns[j])==0){			
			
			set = 1;

		}
		if(set == 1)
		{
			pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *)*(k+1));
			pipe_cmds[k++] = NULL;
			dup2(fdin,0);
			close(fdin);
	
			if(last != 1)
			{
				int fdpipe[2];
				pipe(fdpipe);
				fdout = fdpipe[1];
				fdin = fdpipe[0];
			}
									
			dup2(fdout,1);
			close(fdout);

			if((pid=fork())<0){
				perror("fork");
			}
			else if(pid==0){
				if(execvp(pipe_cmds[0],pipe_cmds)<0){
					perror("exec");
					exit(0);
				}
			}
			else{
				do{
					waitpid(pid,&status,WUNTRACED);
				}while(!WIFEXITED(status) && !WIFSIGNALED(status));
				close(fdout);
			}
			if(last == 1)
				break;
			k = 0;
			pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *)*1);
			cmd_table[i].pipe -= 1;	
			
		}
		else
		{
			pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *) * (j + 1));
			pipe_cmds[k] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
			strcpy(pipe_cmds[k],cmd_table[i].cmdtkns[j]);	
			k = k + 1;
		}
	}
			
	dup2(tmpin,0);
	dup2(tmpout,1);
	close(tmpin);
	close(tmpout);
	for(i=0;i<k;i++)
		free(pipe_cmds[i]);
	free(pipe_cmds);
}
