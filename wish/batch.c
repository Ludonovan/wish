#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include "exec.h"

#define MAX_LINE 50
#define MAX_ARGS 20

extern char *next_arg;
extern int lineNum;
extern char error_message[30];

char *args[MAX_ARGS] = { NULL };
char *PATH[MAX_ARGS] = { NULL };
int num_args; 
int found_at = -1; // where in args '>' is

void check_redir(char *token, int token_length) { 
	int redir_found = 0; // where in token redir is
	int redir_in_args;
    while (token != NULL && redir_found < token_length && token[redir_found] != '\n') {
	    if (token[redir_found] == '>') {
            char tmp[MAX_LINE] = {'\0'};
            int tmp_index = 0;
		    for (int token_index = 0; token_index <= redir_found; token_index++) { // make string of chars before '>'
		        if (token_index != redir_found && token[token_index] != ' ') { // not a space 
                    tmp[tmp_index] = token[token_index];
                    if (token_index != redir_found - 1) { tmp_index++; } // dont increase token index if at char before '>'
                } else { // anything but a space
                    int tmp_len = strlen(tmp);
                    args[num_args] = malloc(tmp_len);
		            strncpy(args[num_args], tmp, tmp_len); 
                    num_args++;
                    tmp_index = 0;
                    for (int j = 0; j < tmp_len + 1; j++) { tmp[j] = '\0'; } 
                }
		    }
            found_at = num_args;
            redir_in_args = found_at;
		    int tmp2_index = redir_found;
            while (token[tmp2_index] == ' ' && tmp2_index < token_length) { tmp2_index++; }
            int k = 0;
		    char tmp2[MAX_LINE] = {'\0'};
		    while (tmp2_index < token_length && token[tmp2_index] != '\n') {
                if (/*token[tmp2_index] != '\n' && */token[tmp2_index] != ' ') {
                    tmp2[k] = token[tmp2_index]; 
		            tmp2_index++; 
                    k++;
                    if (token[tmp2_index - 1] == '>') {
                        args[num_args] = malloc(strlen(tmp2));
                        strncpy(args[num_args], tmp2, strlen(tmp2));
                        num_args++;
                        tmp2_index++;
                        k = 0;
                        for (int c = 0; c < tmp2_index + 1; c++) { tmp2[c] = '\0'; } 
                    }
                } else if (token[tmp2_index] == ' ' && tmp2 != '\0') {
                    int tmp2_len = strlen(tmp2);
                    args[num_args] = malloc(tmp2_len);
                    strncpy(args[num_args], tmp2, tmp2_len);
                    num_args++;
                    tmp2_index++;
                    k = 0;
                    for (int c = 0; c < tmp2_len + 1; c++) { tmp2[c] = '\0'; } 
                /*} else if (token[tmp2_index] == '>') {
                    args[num_args] = malloc(strlen(tmp2));
                    strncpy(args[num_args], tmp2, strlen(tmp2));
                    num_args++;
                    tmp2_index++;
                    //k = 0;*/
                } else if (token[tmp2_index] == '\0' || token[tmp2_index] == '\n') {
                    if (tmp2[0] != '\0' && tmp2[0] != '\n') {
                        args[num_args] = malloc(strlen(tmp2));
                        strncpy(args[num_args], tmp2, strlen(tmp2));
                    }
                    tmp2_index = token_length;
                    token = NULL;
                } 
            }
	    } else if (redir_in_args != found_at) {
            redir_found++;
        }
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
    strcpy(PATH[0], "/bin/"); 
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
   
