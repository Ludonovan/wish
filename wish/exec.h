extern int num_args;
extern char error_message[30];

void exec(char **exec_args);
void print_error();
void exec_cmd(char *PATH, char **exec_args);
void exec_cd(char **exec_args);
//void exec_path(char *PATH);
void exec_exit();
