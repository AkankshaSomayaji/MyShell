/* ------------------------------ Includes ------------------------------- */

#include "../include/main.h"

/* ---------------------------- Main Function ---------------------------- */

int main(int argc, char ** argv, char ** envp){
    int i = 0;
    char * input = (char *)calloc(sizeof(char), MAX_INPUT);
    char ** tokens = NULL;
    int num_tokens = 0;
    greet();
    environ = envp;
    while(true){
        prompt();
        num_tokens = 0;

        memset(input, '\0', MAX_INPUT);
        fgets(input, MAX_INPUT, stdin);

        if(strlen(input) == 1){
            continue;
        }
        else{
            tokenize(input, &tokens, &num_tokens);
        }
        if(strcmp(tokens[0], "exit") == 0){
            clean_up(input, tokens, num_tokens);
            return 0;
        }
		if(strcmp(tokens[0],"alias")==0){
			set_alias_table(num_tokens,tokens);
			continue;
		}
		int ret = check(tokens[0]);
		if(ret>=0){
			printf("ret: %d\n",ret);
			char ** cpy_tokens = NULL;
			cpy_tokens = (char **)malloc(sizeof(char *) * 1);
			int k;
			int ct = 0;
			for(k = 0;k < alias_table[ret].tok_num;k++)
			{
				cpy_tokens= (char **)realloc(cpy_tokens,sizeof(char *) * (k + 1));
				cpy_tokens[k] = (char *)malloc(sizeof(char) * (strlen(alias_table[ret].a_tok[k]) + 1));
				strcpy(cpy_tokens[k],alias_table[ret].a_tok[k]);	
			}
			ct = k-1;
			for(k = 1; k < num_tokens; k++)
			{
				cpy_tokens= (char **)realloc(cpy_tokens,sizeof(char *) * (k + ct+1));
				cpy_tokens[ct + k] = (char *)malloc(sizeof(char) * (strlen(tokens[k]) + 1));
				strcpy(cpy_tokens[ct + k],tokens[k]);	
			}
			num_tokens = ct +k;
			tokens = cpy_tokens;
			for(int j = 0; j < num_tokens  ; j++){
			printf("%s\n",tokens[j]);
			}
		}
		
        //print_tokens(tokens, num_tokens);
        make_cmdtable(tokens,num_tokens);
        //print_cmdtable();
        execute(no_cmd);
		
        for(i = 0; i < num_tokens; i++){
            free(tokens[i]);
        }
        free(tokens);
    }
    return 0;
}


/* ------------------------ Function Definitions ------------------------ */

void greet(){
    printf("\n\n\t\t=============================\n\n\t\t| Hi there! Welcome to dug! |\n\t\t  \\ ^___^\n\t\t    (o o)\\___________\n\t\t    (___)\\           )\\/\\\n\t\t      U  | |______| |\n\t\t        _|_|     _|_|\n\t\t=============================\n\n\t\t\t Version 1.0.0 \n\n");
    return;
}

void prompt(){
    char hostname[2048], cwd[2048], cwd_dir[2048], temp;
    int i = 0, j = 0;
    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, 1024);
    for(i = strlen(cwd) - 1; i > 0; i--){
        if(cwd[i] == '/'){
            cwd_dir[j] = '\0';
            break;
        }
        cwd_dir[j++] = cwd[i];
    }
    for(i = 0; i < strlen(cwd_dir)/2; i++){
        temp = cwd_dir[i];
        cwd_dir[i] = cwd_dir[strlen(cwd_dir) - i - 1];
        cwd_dir[strlen(cwd_dir) - i - 1] = temp; 
    }
	printf("%s:%s > ", getenv("LOGNAME"), cwd_dir);
    return;
}

void clean_up(char * input, char ** tokens, int num_tokens){
    int i = 0, j = 0;
    free(input);
    for(i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
    for(i = 0; i < no_cmd; i++){
        for(j = 0; j < cmd_table[i].num_tokens; j++){
            free(cmd_table[i].cmdtkns[j]);
        }
        free(cmd_table[i].cmdtkns);
        free(cmd_table[i].infile);
        free(cmd_table[i].outfile);
    }
    return;
}

int check(char *tok){
	for (int i =0;i<=alias_num;i++){
		if(strcmp(alias_table[i].a_val[0],tok)==0)
			return i;
	}
return -1;
}

void set_alias_table(int num, char **tok){
	int index = alias_num+ 1;
	int i,j;
	alias_table[index].a_val = (char **)malloc(sizeof(char *) * 1);
	for(i = 1; i <num;i++){
		if(strcmp(tok[i],"=")==0)
			break;
		alias_table[index].a_val = (char **)realloc(alias_table[index].a_val,sizeof(char *) * (i));
		alias_table[index].a_val[i-1] = (char *)malloc(sizeof(char) * (strlen(tok[i]) + 1));
		strcpy(alias_table[index].a_val[i-1],tok[i]);	
	}
	alias_table[index].val_num=i - 1;
	printf("num is :%d\n",alias_table[index].val_num);

	alias_table[index].a_tok = (char **)malloc(sizeof(char *) * 1);
	j=i+2;
	int ct = 0;
	for(i = j; i < num;i++){
		alias_table[index].a_tok = (char **)realloc(alias_table[index].a_tok,sizeof(char *) * (ct + 1));
		alias_table[index].a_tok[ct] = (char *)malloc(sizeof(char) * (strlen(tok[i]) + 1));
		strcpy(alias_table[index].a_tok[ct],tok[i]);	
		ct = ct + 1;
	}
	alias_table[index].tok_num=i-j-1;

	printf("\ntok:\n");
	for(j = 0; j < alias_table[index].tok_num ; j++){
				printf("%s\n",alias_table[index].a_tok[j]);
	}
	printf("num is :%d\n",alias_table[index].val_num);
	printf("\nval:\n");
	for(j = 0; j < alias_table[index].val_num ; j++){
				printf("%s\n",alias_table[index].a_val[j]);
	}
	alias_num+=1;
	return;
}
