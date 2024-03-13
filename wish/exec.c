#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include "batch.h"

extern int num_args;

extern char error_message[30];

char *next_arg;

void print_error(){ write(STDERR_FILENO, error_message, strlen(error_message)); }

// TODO allow for changing of PATH via path command
#define MAX_PATH 20
char *PATH[MAX_PATH] = { NULL };

void exec_cmd(char *PATH[MAX_PATH], char **exec_args) { // execute other commands
    int found = 0;
    while (access(PATH[found], X_OK) != 0) { 
	found++;
    }
    if (execv(PATH[found], exec_args) == -1) { 
        print_error();
	next_arg = NULL;
        exit(1);
    } 
    next_arg = NULL;
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
    }
}

void exec_path(char **exec_args) { // path
    if (exec_args[1] != NULL) { // has args
        for (int j = 0; j < num_args - 1; j++) {
	    PATH[j] = malloc(strlen(exec_args[j]) + 1);
	    strcpy(PATH[j], exec_args[j]);
	}
    } else if (exec_args[1] == NULL) { // no args
        for (int i = 0; i < MAX_PATH - 1; i++) { 
            PATH[i] = '\0';
        }
    }
}


void exec_exit(char **exec_args) { // exit
    if (exec_args[1] == NULL) { // no args
	exit(0);
    } else { 
	print_error();
        next_arg = NULL;
        exit(0);
    }
}

void exec(char **exec_args) {
    PATH[0] = malloc(strlen("/bin/" + 5));
    strcpy(PATH[0], "/bin/");
    strcat(PATH[0], exec_args[0]);
   
    if (strcmp(exec_args[0], "cd") == 0) {
        exec_cd(exec_args);
    } else if (strcmp(exec_args[0], "path") == 0) {
        exec_path(exec_args);	
    } else if (strcmp(exec_args[0], "exit") == 0) {
        exec_exit(exec_args);
    } else {
	// TODO if not built in, make path empty.
        int rc = fork();
        if (rc < 0) {
	    print_error();
	    exit(1);
        } else if (rc == 0) {
            if (access(*PATH, X_OK) == 0) {
		exec_cmd(PATH, exec_args);
	    } else {
		char *no_args[2] = { "/", NULL };
		exec_cmd(no_args, exec_args);
	    }
        } else {
	    wait(NULL);
        }
    }
    for (int i = 0; i < 20; i++) {
        PATH[i] = '\0';
        free(PATH[i]);
    }
}

