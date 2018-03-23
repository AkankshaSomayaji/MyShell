/* ------------------------------ Includes ------------------------------- */

#include "../include/cmdtable.h"
#include "../include/tokens.h"

/* ------------------------------ Global Variables ------------------------------- */

int no_cmd = 0;
struct command_table cmd_table[25];

/* ------------------------ Function Definitions ------------------------ */

void make_cmdtable(char ** tokens, int num_tokens){
	
	int i, j = 0;
	char def_in[] = "stdin";
	char def_out[] = "stdout";

	if(no_cmd == 25){
		shift_table();
	}

	cmd_table[no_cmd].cmdtkns = (char **)malloc(sizeof(char *) * 1);

	cmd_table[no_cmd].infile = (char *)malloc(sizeof(char) * (strlen(def_in) + 1));
	strcpy(cmd_table[no_cmd].infile, def_in);

	cmd_table[no_cmd].outfile = (char *)malloc(sizeof(char) * (strlen(def_out) + 1));
	strcpy(cmd_table[no_cmd].outfile, def_out);

	cmd_table[no_cmd].num_tokens = 0;

	for(i = 0; i < num_tokens; i++){

		if(strcmp("<", tokens[i]) == 0){
			cmd_table[no_cmd].infile = (char *)realloc(cmd_table[no_cmd].infile, strlen(tokens[i+1]));
			strcpy(cmd_table[no_cmd].infile , tokens[i+1]);
		}

		if(strcmp(">", tokens[i]) == 0){
			cmd_table[no_cmd].outfile = (char *)realloc(cmd_table[no_cmd].outfile,strlen(tokens[i+1]));
			strcpy(cmd_table[no_cmd].outfile , tokens[i+1]);
		}

		cmd_table[no_cmd].cmdtkns = (char **)realloc(cmd_table[no_cmd].cmdtkns,sizeof(char *) * (i + 1));
		cmd_table[no_cmd].cmdtkns[j] = (char *)malloc(sizeof(char) * (strlen(tokens[i]) + 1));
		strcpy(cmd_table[no_cmd].cmdtkns[j],tokens[i]);	
		cmd_table[no_cmd].num_tokens = cmd_table[no_cmd].num_tokens + 1;	
		j = j + 1;
	}
	no_cmd = no_cmd + 1;

}		

void shift_table(){
	int i = 0, j = 0;
	for(i = 0; i < 24; i++){
		for(j = 0; j < cmd_table[i].num_tokens; j++){
			free(cmd_table[i].cmdtkns[j]);
		}
		free(cmd_table[i].cmdtkns);
		free(cmd_table[i].infile);
		free(cmd_table[i].outfile);

		cmd_table[i].cmdtkns = (char **)malloc(sizeof(char *) * cmd_table[i + 1].num_tokens);

		for(j = 0; j < cmd_table[i + 1].num_tokens; j++){
			cmd_table[i].cmdtkns[j] = (char *)malloc(sizeof(char) * (strlen(cmd_table[i + 1].cmdtkns[j]) + 1));
			strcpy(cmd_table[i].cmdtkns[j], cmd_table[i + 1].cmdtkns[j]);
		}

		cmd_table[i].infile = (char *)malloc(sizeof(char) * (strlen(cmd_table[i + 1].infile) + 1));
		cmd_table[i].outfile = (char *)malloc(sizeof(char) * (strlen(cmd_table[i + 1].outfile) + 1));

		strcpy(cmd_table[i].infile, cmd_table[i + 1].infile);	
		strcpy(cmd_table[i].outfile, cmd_table[i + 1].outfile);

		cmd_table[i].num_tokens = cmd_table[i + 1].num_tokens;
	}
	
	for(i = 0; i < cmd_table[24].num_tokens; i++){
		free(cmd_table[24].cmdtkns[i]);
	}

	free(cmd_table[24].cmdtkns);
	free(cmd_table[24].infile);
	free(cmd_table[24].outfile);
	cmd_table[i].num_tokens = 0;

	no_cmd = 24;
	
	return ;
}

void print_cmdtable()
{
	int i = 0 , j = 0;
	for(i = 0; i<no_cmd;i++)
	{
		printf("Command :%d\n",i);
		for(j = 0; j < cmd_table[i].num_tokens ; j++)
		{
			printf("%s\t",cmd_table[i].cmdtkns[j]);
		}
		printf("\nin file is : %s\n",cmd_table[i].infile);
		printf("\nout file is : %s\n",cmd_table[i].outfile);	
	}
}
