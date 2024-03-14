#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "exec.h"

#define MAX_LINE 100
#define MAX_ARGS 20

extern char *next_arg;
extern int lineNum;
extern char error_message[30];

char *args[MAX_ARGS] = { NULL };
char *PATH[MAX_ARGS] = { NULL };
int num_args; 
int found_at = -1;
int valid_redir = 0;

void check_redir(char *token, int token_length) { 
	int redir_found = 0;
	while (token != NULL && redir_found < token_length) {
	    if (token[redir_found] != '\0' && token[redir_found] == '>') {
            char tmp[MAX_ARGS] = {'\0'};
            found_at = redir_found;
		    for (int i = 0; i < redir_found; i++) {
		        if (token[i] != ' ') { 
                    tmp[i] = token[i];
                } else {
                    args[num_args] = malloc(strlen(tmp));
		            strncpy(args[num_args], tmp, strlen(tmp));
                    num_args++;
                }
		    }

            args[num_args] = malloc(strlen(tmp));
		    strncpy(args[num_args], ">", strlen(">"));
            num_args++;
		    int j = found_at + 1;
            int k = 0;
		    char tmp2[MAX_ARGS] = {'\0'};
            int redir_err = 0;
		    while (token[j] != '\0' && j < token_length - 1) {
		        if (token[j] != '\n') {
                    tmp2[k] = token[j];
		            j++; 
                    k++;
                } else if (token[j] == '>') {
                    redir_err = 1;
                    write(STDERR_FILENO, error_message, sizeof(error_message));
                    exit(0);
                } else if (token[j] == ' ') {
                    num_args++;
                    j++;
                    k++;
                } else { 
                    args[num_args] = malloc(strlen(tmp2));
                    strncpy(args[num_args], tmp2, strlen(tmp2));

                }
		    }
            if (args[num_args] != NULL && strcmp(args[num_args], "") != 0 
                    && args[num_args + 1] == NULL && redir_err == 0) {
                valid_redir = 1;
            }
	    }
        if (found_at != redir_found) {
	        redir_found++;
        } else { break; }
	}

}


// Parse the next line of the file and store contents in args array
void parse(FILE *file) {
    int index = 0;
    while(args[index] != NULL) { 
        args[index] = NULL;
        index++;
    }
    char *line = NULL;
    size_t len = 0;
    char *token = NULL;
    int token_length = 0;
    num_args = 0;

    if (getline(&line, &len, file) == -1) {
        // End of file reached
        free(line);
        exit(0);
    }
    check_redir(line, strlen(line));	

    // Tokenize the line
    token = strtok(line, " \t \n >");

    while (token != NULL && num_args < MAX_ARGS - 1 && found_at == -1) {
        token_length = strlen(token);
        args[num_args] = malloc(token_length);
        check_redir(token, token_length);
        strncpy(args[num_args], token, token_length);
        next_arg = args[num_args];
        num_args++;
        token = strtok(NULL, " \t \n");
        
    }

    free(line); 
}



// batch mode
int batch(char *filename) {
    FILE *file;
    file = fopen(filename, "r");
    if (file == NULL) {
        write(STDERR_FILENO, error_message, strlen(error_message));	
        fclose(file);
      	return 1;
    }
/**    
    // Counts number of lines in a file
    char line[MAX_LINE];
    while (fgets(line, MAX_LINE, file) != NULL) {
        printf("line%d: %s", lineNum, line);
        lineNum++;
    }
*/
    
    PATH[0] = malloc(strlen("/bin/") + 1);
    strcpy(PATH[0], "/bin/"); // why is this wrong? seems to be working ok
    do {
        parse(file);
	    if (strcmp(args[0], "path") != 0 && PATH[0] != NULL) {
            strcat(PATH[0], args[0]);
	    }
        exec(args);
    }
    while (args[0] != NULL && next_arg != NULL);
    
    for (int i = 0; i < num_args; i++) {
        free(args[i]);
    }
    strcpy(PATH[0], "/bin/");
    lineNum = 0;
    fclose(file);
    return 0;
}
   
