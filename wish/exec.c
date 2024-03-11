#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "batch.h"

extern int num_args;

extern char error_message[30];

char *next_arg;

void print_error(){ write(STDERR_FILENO, error_message, strlen(error_message)); }

// TODO allow for changing of PATH via path command
char PATH[100] = "/bin/";

void exec_cmd(char *PATH, char **exec_args) { //execute other commands
    if (execv(PATH, exec_args) == -1) { 
        print_error();
        exit(1);
    }
}

void exec_cd(char **exec_args) { // cd
    if (exec_args[1] == NULL || exec_args[2] != NULL) { // cd should only take one argument 
        next_arg = NULL;
        print_error();
        exit(0);
    } else if (chdir(exec_args[1]) == -1) { // arg not found
            print_error();
	    next_arg = NULL;
            exit(0);
    } else { 
        next_arg = exec_args[num_args];
    }
}

// TODO write void exec_path();

void exec_exit(char **exec_args) {
    if (exec_args[1] == NULL) {
	exit(0);
    } else { 
	//print_error();
        next_arg = NULL;
        exit(0);
    }
}

void exec(char **exec_args) {
    strcat(PATH, exec_args[0]);
    
    if (strcmp(exec_args[0], "cd") == 0) {
        exec_cd(exec_args);
    }
    else if (strcmp(exec_args[0], "exit") == 0) {
        exec_exit(exec_args);	
    } else {
        exec_cmd(PATH, exec_args);
    }
    for (int i = 0; i < 100; i++) 
        PATH[i] = '\0';
    strcpy(PATH, "/bin/");
}

