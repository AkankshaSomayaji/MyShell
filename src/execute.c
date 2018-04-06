/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"
#include "../include/cmdtable.h"


/* ------------------------ Function Definitions ------------------------ */

void execute(int num){

		int i = num - 1, j =0;


		for(j=0;j<cmd_table[i].num_tokens;j++)
		{
			glob_t glob_buf;
			int glob_ct = 0;
			if(strchr(cmd_table[i].cmdtkns[j],'*') !=NULL || strchr(cmd_table[i].cmdtkns[j],'?')!=NULL)
			{
				char *temp = malloc(sizeof(char *)*strlen(cmd_table[i].cmdtkns[j]));
				strcpy(temp,cmd_table[i].cmdtkns[j]);
				free(cmd_table[i].cmdtkns[j]);
				cmd_table[i].cmdtkns = (char **)realloc(cmd_table[i].cmdtkns,sizeof(char *)*(cmd_table[i].num_tokens-1));
				cmd_table[i].num_tokens= cmd_table[i].num_tokens-1;
				glob(temp,0,NULL,&glob_buf);
				glob_ct = glob_buf.gl_pathc;
				printf(" it %s",temp);
				for(int k =0; k <glob_ct;k++)
				{
					cmd_table[i].cmdtkns = (char **)realloc(cmd_table[i].cmdtkns,sizeof(char *)*(cmd_table[i].num_tokens+1));
					cmd_table[i].cmdtkns[cmd_table[i].num_tokens] = (char *)malloc(sizeof(char *)*strlen(glob_buf.gl_pathv[k])+1);
					/*temp = strcat(temp,glob_buf.gl_pathv[k]);
					if(k!=glob_ct-1)
						temp = strcat(temp," ");
					cmd_table[i].cmdtkns[j] = (char *)realloc(cmd_table[i].cmdtkns[j],strlen(temp)+1);*/
					strcpy(cmd_table[i].cmdtkns[cmd_table[i].num_tokens],glob_buf.gl_pathv[k]);
					cmd_table[i].num_tokens += 1;
				}
			}
		}
				
		//print_cmdtable();
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
		else if(cmd_table[i].chainop != 0)
		{
			execute_chain_command(num);
		}
		else if(cmd_table[i].andop != 0)
		{
			execute_and_command(num);
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
	int i = num - 1, status, j = 0,io_red=0;
	pid_t pid;
	int ct = 0;
	char ** cmds = (char **)malloc(sizeof(char *) * 1);
	for(j = 0; j < cmd_table[i].num_tokens; j++)
	{
		if(io_red != 0)
		{
			io_red = 0;
			continue;
		}

		if(strcmp("<", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].infile = (char *)realloc(cmd_table[i].infile, strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].infile , cmd_table[i].cmdtkns[j+1]);
			io_red = 1;	
			continue;
		}

		if(strcmp(">", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].outfile , cmd_table[i].cmdtkns[j+1]);
			cmd_table[i].out_append = 0;
			io_red = 1;
			continue;
		}

		if(strcmp(">>", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].outfile , cmd_table[i].cmdtkns[j+1]);
			cmd_table[i].out_append = 1;
			io_red = 1;
			continue;
		}

		if(strcmp("2>", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].errfile , cmd_table[i].cmdtkns[j+1]);
			io_red = 1;
			continue;
		}

		if(strcmp("2>&1", cmd_table[i].cmdtkns[j]) == 0){	
			cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(cmd_table[i].outfile)+1);
			strcpy(cmd_table[i].errfile , cmd_table[i].outfile);
			continue;
		}

		cmds = (char **)realloc(cmds,sizeof(char *) * (ct + 1));
		cmds[ct] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
		strcpy(cmds[ct],cmd_table[i].cmdtkns[j]);
		ct = ct + 1;		
	}
	cmds = (char **)realloc(cmds,sizeof(char *)*(ct+1));
	cmds[ct] = NULL;
	
	/*int g = 0;
	for(g=0;g<ct;g++)
		printf("%s\t",cmds[g]);*/

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
		if(execvp(cmds[0],cmds)<0){
			perror("exec");
			exit(1);
		}
	}
	else if(pid>0){
		pid = waitpid(pid,&status,0);		
	}
	for(i=0;i<ct;i++)
		free(cmds[ct]);
	free(cmds);
}

void execute_pipe_command(int num)
{
	int i = num - 1, status,io_red=0;
	pid_t pid;
	int ct = 0;
	char ** pipe_cmds = malloc(sizeof(char *) * 1);
	int k = 0, j = 0 , set = 0 , last = 0;
	char ** cmds = (char **)malloc(sizeof(char *) * 1);
	for(j = 0; j < cmd_table[i].num_tokens; j++)
	{
		if(io_red != 0)
		{
			io_red = 0;
			continue;
		}

		if(strcmp("<", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].infile = (char *)realloc(cmd_table[i].infile, strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].infile , cmd_table[i].cmdtkns[j+1]);
			io_red = 1;	
			continue;
		}

		if(strcmp(">", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].outfile , cmd_table[i].cmdtkns[j+1]);
			cmd_table[i].out_append = 0;
			io_red = 1;
			continue;
		}

		if(strcmp(">>", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].outfile , cmd_table[i].cmdtkns[j+1]);
			cmd_table[i].out_append = 1;
			io_red = 1;
			continue;
		}

		if(strcmp("2>", cmd_table[i].cmdtkns[j]) == 0){
			cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(cmd_table[i].cmdtkns[j+1])+1);
			strcpy(cmd_table[i].errfile , cmd_table[i].cmdtkns[j+1]);
			io_red = 1;
			continue;
		}

		if(strcmp("2>&1", cmd_table[i].cmdtkns[j]) == 0){	
			cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(cmd_table[i].outfile)+1);
			strcpy(cmd_table[i].errfile , cmd_table[i].outfile);
			continue;
		}

		cmds = (char **)realloc(cmds,sizeof(char *) * (ct + 1));
		cmds[ct] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
		strcpy(cmds[ct],cmd_table[i].cmdtkns[j]);
		ct = ct + 1;		
	}
	cmds = (char **)realloc(cmds,sizeof(char *)*(ct+1));
	cmds[ct] = NULL;

	int tmpin = dup(0);
	int tmpout = dup(1);



	int fdin;
	if(strcmp("stdin",cmd_table[i].infile)!=0){
		fdin = open(cmd_table[i].infile,O_RDONLY,0644);
	}
	else{
		fdin = dup(tmpin);
	}
	int fdout;
	for(j = 0; j < ct; j++)
	{
		set = 0;
		if(cmd_table[i].pipe == 0)
		{
			k = 0;
			pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *)*1);
			int t;
			for(t = j; t < ct; t++){
				pipe_cmds = (char **)realloc(pipe_cmds,sizeof(char *) * (t + 1));
				pipe_cmds[k] = (char *)malloc(sizeof(char)*(strlen(cmds[t])+ 1));
				strcpy(pipe_cmds[k],cmds[t]);
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
		if(strcmp("|",cmds[j])==0){			
			
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
			pipe_cmds[k] = (char *)malloc(sizeof(char) * (strlen(cmds[j]) + 1));
			strcpy(pipe_cmds[k],cmds[j]);	
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
void execute_and_command(int num)
{
	//printf("\n inside qwerty");
	char ** and_cmds = malloc(sizeof(char *) * 1);
	int k = 0, j = 0 , z = 0, set = 0 , last = 0, io_red=0;

	int i = num - 1 , status = 0;
	pid_t pid;
	for(j = 0; j < cmd_table[i].num_tokens; j++)
	{
		set = 0;
		if(cmd_table[i].andop == 0)
		{
			k = 0;
			and_cmds = (char **)realloc(and_cmds,sizeof(char *)*1);
			int t;
			for(t = j; t < cmd_table[i].num_tokens; t++){
				and_cmds = (char **)realloc(and_cmds,sizeof(char *) * (t + 1));
				and_cmds[k] = (char *)malloc(sizeof(char)*(strlen(cmd_table[i].cmdtkns[t])+ 1));
				strcpy(and_cmds[k],cmd_table[i].cmdtkns[t]);
				k = k + 1;
			}
			set = 1;	
			last = 1;
		}		
		if(strcmp("&&",cmd_table[i].cmdtkns[j])==0){			
			
			set = 1;

		}
		if(set == 1)
		{
			char ** cmds = (char **)malloc(sizeof(char *) * 1);
			int ct = 0;
			for(z = 0; z < k; z++)
			{
				if(io_red != 0)
				{
					io_red = 0;
					continue;
				}

				if(strcmp("<", and_cmds[z]) == 0){
					cmd_table[i].infile = (char *)realloc(cmd_table[i].infile, strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].infile , and_cmds[z+1]);
					io_red = 1;	
					continue;
				}

				if(strcmp(">", and_cmds[z]) == 0){
					cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].outfile , and_cmds[z+1]);
					cmd_table[i].out_append = 0;
					io_red = 1;
					continue;
				}

				if(strcmp(">>", and_cmds[z]) == 0){
					cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].outfile , and_cmds[z+1]);
					cmd_table[i].out_append = 1;
					io_red = 1;
					continue;
				}

				if(strcmp("2>", and_cmds[z]) == 0){
					cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].errfile , and_cmds[z+1]);
					io_red = 1;
					continue;
				}

				if(strcmp("2>&1", and_cmds[z]) == 0){	
					cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(cmd_table[i].outfile)+1);
					strcpy(cmd_table[i].errfile , cmd_table[i].outfile);
					continue;
				}

				cmds = (char **)realloc(cmds,sizeof(char *) * (ct + 1));
				cmds[ct] = (char *)malloc(sizeof(char) * (strlen(and_cmds[z]) + 1));
				strcpy(cmds[ct],and_cmds[z]);
				ct = ct + 1;		
			}
			cmds = (char **)realloc(cmds,sizeof(char *)*(ct+1));
			cmds[ct] = NULL;
			
			if(status == 0)
			{
				int fd = -1;
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
						//printf("\nhello\n");
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
					if(execvp(cmds[0],cmds)<0){
						perror("exec");
						exit(-1);
					}
				}
				else{
					pid = waitpid(pid,&status,WUNTRACED);
					cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen("stdout")+1);
					strcpy(cmd_table[i].errfile , "stdout");
					cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen("stdout")+1);
					strcpy(cmd_table[i].outfile , "stdout");
					cmd_table[i].infile = (char *)realloc(cmd_table[i].infile,strlen("stdin")+1);
					strcpy(cmd_table[i].infile , "stdin");
				}
			}
			if(last == 1)
				break;
			k = 0;
			and_cmds = (char **)realloc(and_cmds,sizeof(char *)*1);
			cmd_table[i].andop -= 1;	
			
		}
		else
		{
			and_cmds = (char **)realloc(and_cmds,sizeof(char *) * (j + 1));
			and_cmds[k] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
			strcpy(and_cmds[k],cmd_table[i].cmdtkns[j]);	
			k = k + 1;
		}
	}
	for(i=0;i<k;i++)
		free(and_cmds[i]);
	free(and_cmds);
}

void execute_chain_command(int num)
{
	//printf("\n inside qwerty");
	char ** and_cmds = malloc(sizeof(char *) * 1);
	int k = 0, j = 0 , z = 0, set = 0 , last = 0, io_red=0;

	int i = num - 1 , status = 0;
	pid_t pid;
	for(j = 0; j < cmd_table[i].num_tokens; j++)
	{
		set = 0;
		if(cmd_table[i].chainop == 0)
		{
			k = 0;
			and_cmds = (char **)realloc(and_cmds,sizeof(char *)*1);
			int t;
			for(t = j; t < cmd_table[i].num_tokens; t++){
				and_cmds = (char **)realloc(and_cmds,sizeof(char *) * (t + 1));
				and_cmds[k] = (char *)malloc(sizeof(char)*(strlen(cmd_table[i].cmdtkns[t])+ 1));
				strcpy(and_cmds[k],cmd_table[i].cmdtkns[t]);
				k = k + 1;
			}
			set = 1;	
			last = 1;
		}		
		if(strcmp(";",cmd_table[i].cmdtkns[j])==0){			
			
			set = 1;

		}
		if(set == 1)
		{
			char ** cmds = (char **)malloc(sizeof(char *) * 1);
			int ct = 0;
			for(z = 0; z < k; z++)
			{
				if(io_red != 0)
				{
					io_red = 0;
					continue;
				}

				if(strcmp("<", and_cmds[z]) == 0){
					cmd_table[i].infile = (char *)realloc(cmd_table[i].infile, strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].infile , and_cmds[z+1]);
					io_red = 1;	
					continue;
				}

				if(strcmp(">", and_cmds[z]) == 0){
					cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].outfile , and_cmds[z+1]);
					cmd_table[i].out_append = 0;
					io_red = 1;
					continue;
				}

				if(strcmp(">>", and_cmds[z]) == 0){
					cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].outfile , and_cmds[z+1]);
					cmd_table[i].out_append = 1;
					io_red = 1;
					continue;
				}

				if(strcmp("2>", and_cmds[z]) == 0){
					cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(and_cmds[z+1])+1);
					strcpy(cmd_table[i].errfile , and_cmds[z+1]);
					io_red = 1;
					continue;
				}

				if(strcmp("2>&1", and_cmds[z]) == 0){	
					cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen(cmd_table[i].outfile)+1);
					strcpy(cmd_table[i].errfile , cmd_table[i].outfile);
					continue;
				}

				cmds = (char **)realloc(cmds,sizeof(char *) * (ct + 1));
				cmds[ct] = (char *)malloc(sizeof(char) * (strlen(and_cmds[z]) + 1));
				strcpy(cmds[ct],and_cmds[z]);
				ct = ct + 1;		
			}
			cmds = (char **)realloc(cmds,sizeof(char *)*(ct+1));
			cmds[ct] = NULL;
			
			//if(status == 0)
			//{
				//printf("out:%s",cmd_table[i].outfile);
				int fd = -1;
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
						//printf("\nhello\n");
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
					if(execvp(cmds[0],cmds)<0){
						perror("exec");
						exit(-1);
					}
				}
				else{
					pid = waitpid(pid,&status,WUNTRACED);
					cmd_table[i].errfile = (char *)realloc(cmd_table[i].errfile,strlen("stdout")+1);
					strcpy(cmd_table[i].errfile , "stdout");
					cmd_table[i].outfile = (char *)realloc(cmd_table[i].outfile,strlen("stdout")+1);
					strcpy(cmd_table[i].outfile , "stdout");
					cmd_table[i].infile = (char *)realloc(cmd_table[i].infile,strlen("stdin")+1);
					strcpy(cmd_table[i].infile , "stdin");
				}
			//}
			if(last == 1)
				break;
			k = 0;
			and_cmds = (char **)realloc(and_cmds,sizeof(char *)*1);
			cmd_table[i].chainop -= 1;	
			
		}
		else
		{
			and_cmds = (char **)realloc(and_cmds,sizeof(char *) * (j + 1));
			and_cmds[k] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[j]) + 1));
			strcpy(and_cmds[k],cmd_table[i].cmdtkns[j]);	
			k = k + 1;
		}
	}
	for(i=0;i<k;i++)
		free(and_cmds[i]);
	free(and_cmds);
}
