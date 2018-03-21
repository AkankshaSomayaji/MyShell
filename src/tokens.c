/* ------------------------------ Includes ------------------------------- */

#include "../include/tokens.h"

/* ------------------------ Function Definitions ------------------------ */

void tokenize(char * input, char *** tok, int * num_tok){
    int i;

    char ** tokens = (char **)malloc(sizeof(char *) * 1);
    tokens[0] = (char *)calloc(sizeof(char *), 1);
    int num_tokens = 0;

    char * temp = NULL;
    temp = strtok(input, " \n\t");
    tokens[num_tokens] = (char *)realloc(tokens[num_tokens], sizeof(char) * (strlen(temp) + 1));
    strcpy(tokens[num_tokens], temp);
    tokens[num_tokens][strlen(temp)] = '\0';

    num_tokens++;    
    tokens = (char **)realloc(tokens, sizeof(char *) * (num_tokens + 1));
    
	while((temp = strtok(NULL, " \n\t")) != NULL) {
        tokens[num_tokens] = (char *)malloc(sizeof(char) * (strlen(temp) + 1));
        strcpy(tokens[num_tokens], temp);
        tokens[num_tokens][strlen(temp)] = '\0';
        num_tokens++;
        tokens = (char **)realloc(tokens, sizeof(char *) * (num_tokens + 1));
    }

    *tok = (char **)malloc(sizeof(char *) * num_tokens);
    for(i = 0; i < num_tokens; i++){
        (*tok)[i] = (char *)malloc(sizeof(char) * (strlen(tokens[i]) + 1));
        strcpy((*tok)[i], tokens[i]);
        (*tok)[i][strlen(tokens[i])] = '\0';
    }

    *(num_tok) = num_tokens;

    for(i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
    free(temp);
    return ;
}
void make_cmdtable(char ** tokens, int num_tokens)
{
	
	cmd_table[no_cmd].cmdtkns = (char **)malloc(sizeof(char *) * 1);
	int i,j=0;
	cmd_table[no_cmd].infile = (char *)malloc(strlen("stdin"));
	strcpy(cmd_table[no_cmd].infile,"stdin");
	cmd_table[no_cmd].outfile = (char *)malloc(strlen("stdout"));
	strcpy(cmd_table[no_cmd].outfile,"stdout");
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
void print_tokens(char ** tokens, int num_tokens){
    int i = 0;
    for(i = 0; i < num_tokens; i++){
        printf("%s\t", tokens[i]);
    }
    printf("\n");
}
