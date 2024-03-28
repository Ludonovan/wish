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
extern int in_path;
extern char error_message[30];

char *args[MAX_ARGS] = { NULL };
char *PATH[MAX_ARGS] = { NULL };
int num_args; 
int found_at = -1; // where in args '>' is
int path_changed = 0;
int args_index = 0; // used for parallel processes. 
int has_parallel = 0;

void check_parallel() {
    int i = 0;
    while (args[i] != NULL) {
        if (strcmp(args[i], "&") == 0) {
            has_parallel = 1;
            break;
        }
        i++;
    }
    if (i == 0) { exit(0); }

    if (has_parallel == 1) { 
        // TODO handle parallel commands 
        char *p_args[num_args];  
        do {
            if (strcmp(args[args_index], "&") == 0 && args[args_index+1] != NULL) {
                args_index++;
            }
            p_args[0] = malloc(sizeof(args[args_index])); 
            p_args[0] = args[args_index];

            if (strcmp(p_args[0], "&") == 0) {
                exit(0);
            }

            for (int a = 1; a < num_args; a++) { p_args[a] = NULL; } 

            char *old = malloc(sizeof(PATH[0]));
            strcpy(old, PATH[0]);
            strcat(PATH[0], p_args[0]);
            
            if (strcmp(args[args_index], "&") == 0) { 
                args_index++;  
            } else if (args[args_index+1] == NULL) {
                exec(p_args); 
            } else { 
                p_args[0] = args[args_index]; 
                p_args[1] = NULL;
                exec(p_args);
                args_index++; 
            } 
            PATH[0] = NULL;
            PATH[0] = malloc(strlen(old));
            strcpy(PATH[0], old);
        } while(args_index < num_args); 
    }
}


void check_redir(char *token, int token_length) { 
	int redir_found = 0; // where in token redir is
	int redir_in_args;
    int end = 0;
    while (redir_found < token_length && end == 0) {
	    if (token[redir_found] == '>') {
            char tmp[MAX_LINE] = {'\0'};
            int tmp_index = 0;
            int token_index = 0;
		    while (end == 0 && token_index <= redir_found) { // make string of chars before '>'
		        if (token[token_index] != ' ' && token[token_index] != '\n') {  
                    tmp[tmp_index] = token[token_index];
                    if (token_index != redir_found - 1) { 
                        tmp_index++;
                    } else {
                        args[num_args] = malloc(strlen(tmp));
                        strncpy(args[num_args], tmp, strlen(tmp));
                        num_args++;
                        end = 1;
                    }
                } else if (token[token_index] == ' ' ) {  
                    int tmp_len = strlen(tmp);
                    args[num_args] = malloc(tmp_len);
		            strncpy(args[num_args], tmp, tmp_len); 
                    num_args++;
                    tmp_index = 0;
                    for (int j = 0; j < tmp_len + 1; j++) { tmp[j] = '\0'; } 
                } else {
                    end = 1;
                }
                token_index++;
		    }
            found_at = num_args;
            redir_in_args = found_at;
		    int tmp2_index = redir_found;
            while (token[tmp2_index] == ' ' && tmp2_index < token_length) { tmp2_index++; }
            int k = 0;
		    char tmp2[MAX_LINE] = {'\0'};
		    while (end == 0 && tmp2_index < token_length) { 
                if (token[tmp2_index] != '\n' && token[tmp2_index] != ' ') {
                    tmp2[k] = token[tmp2_index]; 
		            tmp2_index++; 
                    k++;
                    if (token[tmp2_index - 1] == '>') {
                        args[num_args] = malloc(strlen(tmp2));
                        strncpy(args[num_args], tmp2, strlen(tmp2));
                        num_args++;
                        k = 0;
                        for (int c = 0; c < tmp2_index + 1; c++) { tmp2[c] = '\0'; } 
                    } 
                } else if (token[tmp2_index] == ' ' && tmp2[0] != '\0') {
                    int tmp2_len = strlen(tmp2);
                    args[num_args] = malloc(tmp2_len);
                    strncpy(args[num_args], tmp2, tmp2_len);
                    num_args++;
                    tmp2_index++;
                    k = 0;
                    for (int c = 0; c < tmp2_len + 1; c++) { tmp2[c] = '\0'; } 
                } else if (token[tmp2_index] == '\n' && tmp2[0] != '\0') {
                    args[num_args] = malloc(strlen(tmp2));
                    strncpy(args[num_args], tmp2, strlen(tmp2));
                    tmp2_index = token_length;
                    end = 1;
                } else {
                    end = 1; 
                } 
            }
        } else if (token[redir_found] == '\n') {
           break; 
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
    token = strtok(line, " \t \n > ");

    while (token != NULL && num_args < MAX_ARGS - 1 && found_at == -1) {
        token_length = strlen(token);
        args[num_args] = malloc(token_length);
        strncpy(args[num_args], token, token_length);
        next_arg = args[num_args];
        num_args++;
        token = strtok(NULL, " \t \n");
        
    }
    if (args[0] == NULL) {
        args[0] = malloc(strlen(" "));
        args[0] = " ";
        next_arg = " ";
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
        check_parallel();
	    if (strcmp(args[0], "path") != 0 && PATH[0] != NULL) {
            int i = 0;
            while (i < in_path && PATH[i] != NULL) {
                strcat(PATH[i], args[0]);
                i++;
                path_changed = 1;
            }
	    }
        if (args[0] != " " && has_parallel == 0)
            exec(args);
        
        if (path_changed != 0 && in_path > 0) {
            for (int i = 0; i < in_path; i++) {
                int chop = strlen(PATH[i]) - strlen(args[0]);
                PATH[i][chop] = '\0';
            }
        }
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
   
