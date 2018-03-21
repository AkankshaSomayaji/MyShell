/* ------------------------------ Includes ------------------------------- */

#include "../include/cmdtable.h"
#include "../include/tokens.h"

/* ------------------------ Function Definitions ------------------------ */

void make_cmdtable(char ** tokens, int num_tokens)
{
	char def_in[]="stdin";
	char def_out[]="stdout";
	cmd_table[no_cmd].cmdtkns = (char **)malloc(sizeof(char *) * 1);
	int i,j=0;
	cmd_table[no_cmd].infile = (char *)malloc(strlen(def_in));
	cmd_table[no_cmd].infile = def_in;
	cmd_table[no_cmd].outfile = (char *)malloc(strlen(def_out));
	cmd_table[no_cmd].outfile = def_out;
	for(i=0;i<num_tokens;i++)
	{
		if(strcmp("<",tokens[i])==0)
		{
			cmd_table[no_cmd].infile = (char *)realloc(cmd_table[no_cmd].infile,strlen(tokens[i+1]));
			strcpy(cmd_table[no_cmd].infile , tokens[i+1]);
		}
		if(strcmp(">",tokens[i])==0)
		{
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
