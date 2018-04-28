/* ------------------------------ Includes ------------------------------- */

#include "../include/execute.h"
#include "../include/tokens.h"
#include "../include/cmdtable.h"



/* ------------------------ Function Definitions ------------------------ */

void execute(int num){

		int i = num - 1, j =0;
		


		/* below for loop is only for replacing wild char (* and ?) */
		for(j=0;j<cmd_table[i].num_tokens;j++)
		{
			int len = cmd_table[i].num_tokens;
			glob_t glob_buf;
			int glob_ct = 0;
			if(strchr(cmd_table[i].cmdtkns[j],'*') !=NULL || strchr(cmd_table[i].cmdtkns[j],'?')!=NULL)
			{
				int k = 0;
				char ** new_cmd = (char **)malloc(sizeof(char *)*1);
				for(k = 0; k < j;k++)
				{
					new_cmd = (char **)realloc(new_cmd,sizeof(char *) * (k + 1));
					new_cmd[k] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[k]) + 1));
					strcpy(new_cmd[k],cmd_table[i].cmdtkns[k]);
				}
				char *temp = malloc(sizeof(char *)*strlen(cmd_table[i].cmdtkns[j]));
				strcpy(temp,cmd_table[i].cmdtkns[j]);
				glob(temp,0,NULL,&glob_buf);
				glob_ct = glob_buf.gl_pathc;
				int x = k;
				int y = k + 1;
				cmd_table[i].num_tokens = cmd_table[i].num_tokens - 1;
				for(k =0; k < glob_ct;k++)
				{
					new_cmd = (char **)realloc(new_cmd,sizeof(char *)*(x+1));
					new_cmd[x] = (char *)malloc(sizeof(char *)*strlen(glob_buf.gl_pathv[k])+1);
					strcpy(new_cmd[x],glob_buf.gl_pathv[k]);
					cmd_table[i].num_tokens += 1;
					x += 1;
				}

				for(k = y; k < len;k++)
				{
					new_cmd = (char **)realloc(new_cmd,sizeof(char *) * (x + 1));
					new_cmd[x] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i].cmdtkns[k]) + 1));
					strcpy(new_cmd[x],cmd_table[i].cmdtkns[k]);
					x += 1;
				}
				cmd_table[i].cmdtkns = new_cmd;
			}
		}
			
		cmd_table[i].cmdtkns = (char **)realloc(cmd_table[i].cmdtkns,sizeof(char *)*(cmd_table[i].num_tokens+1));
		cmd_table[i].cmdtkns[cmd_table[i].num_tokens] = NULL;
		
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

void printdir(const char *name, char *string, int depth)
{
	DIR *dp=NULL;
	int cnt=0;
	struct dirent *entry=NULL;
	if((dp=opendir(name))<0){
		fprintf(stderr,"cannot open directory : %s\n",name);
		return;
	}

	while((entry = readdir(dp)) != NULL){
		if(entry->d_type == DT_REG){		
			char path[1024];
			snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);		
		    cnt=searchfile(path,string);
		    if(cnt>0)	printf("count: %d\n",cnt);
		}
		else if (entry->d_type == DT_DIR){
			char path[1024];
		    if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
		        continue;
		    snprintf(path, sizeof(path), "%s/%s", name, entry->d_name);
			printf("%s: Is a Directory\n",path);
		    printdir(path,string, depth + 2);
		}
	}
	closedir(dp);
}

int searchfile(char *fname, char *str){
	FILE *fp;
	int line_num = 1;
	int find_result = 0;
	char temp[512];
	//printf("fname: %s\n",fname);
	if((fp = fopen(fname, "r")) == NULL) {
		return(-1);
	}
	while(fgets(temp, 512, fp) != NULL) {
		int i=0;
		while(i<strlen(temp)){
			if ((!isascii(temp[i]) || iscntrl(temp[i])) && !isspace(temp[i]))
				return -1;
			i++;
		}
		if((strstr(temp, str)) != NULL) {
			find_result++;
			if(find_result==1) printf("%s:\n",fname);
			printf("%d: ",line_num);		//A match found on this line_num
			printf("%s", temp);			//Prints the line
		}
		line_num++;
	}	
	//Close the file if still open.
	if(fp) {
		fclose(fp);
	}
   	return(find_result);
}

void tree(char * my_name, int spaces)
{
	char * name = (char *)malloc(sizeof(char)* (strlen(my_name) + 1));
	strcpy(name,my_name);
	DIR *dp = NULL;
	if((dp=opendir(name))<0){
		fprintf(stderr,"cannot open directory : %s\n",name);
		return;
	}
	struct dirent * rd;
	rd=readdir(dp);
	if(rd==NULL)
		perror("readdir");
	//struct stat sb;
	
	while(rd!=NULL)
	{
		if(rd->d_type == DT_DIR && strcmp(rd->d_name,".")!=0 && strcmp(rd->d_name,"..")!=0 )
		{
			for(int j = 0; j< spaces; j++)
				printf("\t");
			printf("[%s]\n",rd->d_name);
			char * temp = (char *)malloc(sizeof(char)* (strlen(my_name) + 1));
			strcpy(temp,name);
			name = (char *)realloc(name,sizeof(char) *(strlen(name)+2));
			name = strcat(name,"/");
			name = (char *)realloc(name,sizeof(char) *(strlen(name)+strlen(rd->d_name)+1));
			name = strcat(name,rd->d_name);
			tree(name , spaces + 1);
			name = (char *)realloc(name,sizeof(char) *(strlen(temp)+1));
			strcpy(name,temp);
		}
		else if(strcmp(rd->d_name,".")!=0 && strcmp(rd->d_name,"..")!=0)
		{
			for(int j = 0; j< spaces; j++)
				printf("\t");
			printf("%s\n",rd->d_name);
		}
		rd=readdir(dp);
	}

}



void alrm_hndlr(int sno, siginfo_t *siginfo, void* context)
{
kill(siginfo->si_pid, SIGKILL);
}

void handler(int sno)
{}

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
	
	if(strcmp(cmds[0], "cd") == 0){
		int ret = 0;
		ret = change_directory(cmds);
		getcwd(cwd,1024);
		//printf("\ncwd:%s",cwd);
		if(ret < 0){
			perror("cd");
		}
		else{
			return;
		}
	}

	if(strcmp(cmds[0], "history") == 0){
		char * home_dir = getenv("HOME"); 
		char * path = (char *)calloc(sizeof(char),  (strlen(home_dir) + 2 + strlen("myshell_history")));
		char buf[1];
		strcat(path, home_dir);
		strcat(path, "/");
		strcat(path, "myshell_history");

		int fd = open(path, O_CREAT | O_RDWR, 0644);

		while(read(fd, buf, 1) > 0){
			write(STDOUT_FILENO, buf, 1);
		}

		close(fd);
		return;
	}

	if(strcmp(cmds[0], "sgown") == 0){
			printdir(".",cmds[1],0);
			return;
	}


	if(strcmp(cmds[0], "tree") == 0){
			//char cwd[1024];
			getcwd(cwd,1024);
			//printf("\ncwd:%s",cwd);
			tree(cwd,0);
			return;
	}


	if((pid=fork())<0){
		perror("fork");
	}
	
	else if(pid==0){
		
		//alarm(5);

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
	//printf("\npipe command\n");
	//printf("\nhelllo\n");
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
	int no_history;
	no_history = 0;
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

				//alarm(5);
				if(strcmp(cmds[0], "history") == 0 && no_history == 0){
					//printf("\nhelllo\n");
					char * home_dir = getenv("HOME"); 
					char * path = (char *)calloc(sizeof(char),  (strlen(home_dir) + 2 + strlen("myshell_history")));
					char buf[1];
					strcat(path, home_dir);
					strcat(path, "/");
					strcat(path, "myshell_history");

					int fd = open(path, O_CREAT | O_RDWR, 0644);

					while(read(fd, buf, 1) > 0){
						write(STDOUT_FILENO, buf, 1);
					}

					close(fd);
					no_history = 1;
					//return;
					exit(0);
				}
				else if(execvp(pipe_cmds[0],pipe_cmds)<0){
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
				if(strcmp(cmds[0], "cd") == 0){
				int ret = 0;
				ret = change_directory(cmds);
				getcwd(cwd,1024);
					if(ret < 0){
						perror("cd");
					}
					else{
						if(last == 1)
							break;
						k = 0;
						and_cmds = (char **)realloc(and_cmds,sizeof(char *)*1);
						cmd_table[i].andop -= 1;	
						continue;
					}
				}
				if(strcmp(cmds[0], "tree") == 0){
					//char cwd[1024];
					getcwd(cwd,1024);
					//printf("\ncwd:%s",cwd);
					tree(cwd,0);
					continue;
				}
	
				int fd = -1;
				if((pid=fork())<0){
					perror("fork");
				}
				else if(pid==0){

					//alarm(5);

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
			
			if(strcmp(cmds[0], "cd") == 0){
				int ret = 0;
				ret = change_directory(cmds);
				getcwd(cwd,1024);
					if(ret < 0){
						perror("cd");
					}
					else{
						if(last == 1)
							break;
						k = 0;
						and_cmds = (char **)realloc(and_cmds,sizeof(char *)*1);
						cmd_table[i].chainop -= 1;	
						continue;
					}
			}
			if(strcmp(cmds[0], "tree") == 0){
					//char cwd[1024];
					getcwd(cwd,1024);
					//printf("\ncwd:%s",cwd);
					tree(cwd,0);
					continue;
			}

			int fd = -1;
			if((pid=fork())<0){
				perror("fork");
			}
			else if(pid==0){
				
					//alarm(5);

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
