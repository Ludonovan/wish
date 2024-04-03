#define MAX_COMMANDS 10

extern int lineNum;
extern int num_commands;
extern int has_parallel;
extern char **commands[MAX_COMMANDS];
extern char *next_arg;
extern char error_message[30];
int batch(char *filename);
void parse(FILE *file);
