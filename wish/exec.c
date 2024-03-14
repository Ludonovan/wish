#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "batch.h"

#define MAX_PATH 20

extern int num_args;
extern int found_at;
extern int valid_redir;
extern char error_message[30];
extern char *PATH[MAX_PATH];

char *next_arg;
int found = 0;


void print_error(){ write(STDERR_FILENO, error_message, strlen(error_message)); }

void exec_cmd(char *PATH[MAX_PATH], char **exec_args) { // execute other commands
    char *output = malloc(strlen(exec_args[found_at + 1]) + 2);
    if (exec_args[1] != NULL && strcmp(exec_args[1], ">") == 0) { 
	    if (exec_args[0] == NULL || exec_args[2] == NULL || exec_args[3] != NULL) {
	        print_error();
	        exit(1);
	    } else if (found_at != -1 && valid_redir == 0) {
            close(STDOUT_FILENO);
            //char *output = malloc(strlen(exec_args[found_at + 1]) + 2);
            strcat(output, "./");
            strcat(output, exec_args[found_at + 1]);
            open(output, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
            //free(output);
        }
    }
    
    if (execv(PATH[found], exec_args) != 0) { 
        print_error();
        exit(1);
    } 
    next_arg = NULL;
    free(output);
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

    // make path orginial without "cd"
    strcpy(PATH[0], strtok(PATH[0], "cd"));
}


void exec_path(char **exec_args) { // path
    if (exec_args[1] != NULL) { // has args
        for (int j = 0; j < num_args - 1; j++) {
	        if (PATH[j] != NULL) 
		        free(PATH[j]);
	        PATH[j] = malloc(strlen(exec_args[j+1]) + 3);
	        char *tmp = malloc(strlen(exec_args[j+1]) + 3);
	        //strcpy(tmp, "/");
	        strcat(tmp, exec_args[1]);
	        strcpy(PATH[j], tmp);
	        strcat(PATH[j], "/");
	        free(tmp);
	    }
    } else if (exec_args[1] == NULL) { // no args
        for (int k = 0; k < MAX_PATH - 1; k++) {
	        if (PATH[k] != NULL) 
		        free(PATH[k]);	
            PATH[k] = NULL;
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
    for (int i = 0; i < MAX_PATH; i++) {
    	if (PATH[i] != NULL && access(PATH[i], X_OK) == 0) {
	        found = i;
	    }
    }

    if(exec_args[0] == NULL) {
	    exit(0);
    }

    if (strcmp(exec_args[0], "cd") == 0) {
        exec_cd(exec_args);
    } else if (strcmp(exec_args[0], "path") == 0) {
        exec_path(exec_args);	
    } else if (strcmp(exec_args[0], "exit") == 0) {
        exec_exit(exec_args);
    } else {
	if (found == MAX_PATH) {
	    print_error();
	    exit(0);
	}
        int rc = fork();
        if (rc < 0) {
	        print_error();
	        exit(1);
        } else if (rc == 0) {
            exec_cmd(PATH, exec_args);
        } else {
	        wait(NULL);
        }
    }
}

