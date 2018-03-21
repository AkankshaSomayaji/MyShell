/* --------------------------- Global Variables -------------------------- */
/*
global command structure
*/

struct command_table
{
char ** cmdtkns;
int num_tokens;
char * outfile;
char * infile;
};

/* global command table */
struct command_table cmd_table[25];

/* interger count of commands */
int no_cmd;


/* --------------------------- Function Headers -------------------------- */

/*
construct command table
*/

void make_cmdtable(char ** tokens, int num_tokens);

/*
print command table
*/
void print_cmdtable();
