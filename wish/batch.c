#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#include "exec.h"

extern char *next_arg;

#define MAX_LINE 100
#define MAX_ARGS 20

extern int lineNum;
extern char error_message[30];

char *args[MAX_ARGS] = { NULL };

char *PATH[MAX_ARGS] = { NULL };

int num_args; 

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

    // Tokenize the line
    token = strtok(line, " \n >");

    while (token != NULL && num_args < MAX_ARGS - 1) {
        token_length = strlen(token);
        args[num_args] = malloc(token_length + 1);
        strncpy(args[num_args], token, token_length);
        next_arg = args[num_args];
        num_args++;
        token = strtok(NULL, " \n");
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
    
    PATH[0] = malloc(strlen("/bin/" + 10));
    strcpy(PATH[0], "/bin/");
    do {
        parse(file);
	if (strcmp(args[0], "path") != 0 && PATH[0] != NULL) {
            strcat(PATH[0], args[0]);
	}
        exec(args);
    }
    while (args[0] != NULL && next_arg != NULL);


    strcpy(PATH[0], "/bin/");
    lineNum = 0;
    fclose(file);
    return 0;
}
   
