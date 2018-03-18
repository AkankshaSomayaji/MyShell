/* ------------------------------ Includes ------------------------------- */

#include "../include/main.h"

/* ---------------------------- Main Function ---------------------------- */

int main(int argc, char ** argv, char ** envp){
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
        if(strcmp(tokens[0], "end") == 0){
            return 0;
        }
        print_tokens(tokens, num_tokens);
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
