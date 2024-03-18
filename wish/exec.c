#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "batch.h"
#include "exec.h"

#define MAX_PATH 20
#define MAX_LINE 50

extern int num_args;
extern int found_at;
extern int in_path;
extern int path_changed;
extern char error_message[30];
extern char *PATH[MAX_PATH];

char *next_arg;
int found = 0;

void print_error(){ write(STDERR_FILENO, error_message, strlen(error_message)); }

void exec_cmd(char *PATH[MAX_PATH], char **exec_args) { // execute other commands
    if (exec_args[found_at] != NULL && strcmp(exec_args[found_at], ">") == 0) { 
	    if (exec_args[found_at - 1] == NULL || exec_args[found_at] == NULL || exec_args[found_at + 1] == NULL) {
	        print_error();
	        exit(1);
        } else if (exec_args[found_at + 2] != NULL) {
            print_error();
            exit(1);
        } else if (found_at != -1 && exec_args[found_at + 1] != NULL) {
            close(STDOUT_FILENO);
            char *file_out = exec_args[found_at + 1];
            char *output = malloc(strlen(file_out) + 2);
            // if output is in another directory: dir/file (/tmp/file)
            // the output should be: dir/./file (/tmp/./file), not ./dir/file (.//tmp/file)
            int contains = 0; // 0 if a '/' is not found, 1 if '/' is found.
            int last_found = -1; // where the last '/' is found
            for (int i = 0; i < strlen(file_out); i++) {
                 if (file_out[i] == '/') {
                     last_found = i;
                     contains = 1;
                 }
            }
            if (contains = 1 && last_found != -1) {
                char tmp[MAX_LINE] = { '\0' };
                int tmp_index = 0;
                for (int i = 0; i < last_found; i++) {
                    tmp[tmp_index] = file_out[i];
                }
                strncat(output, tmp, strlen(tmp));
                strcat(output, ".");
                char tmp2[MAX_LINE] = { '\0' };
                int tmp2_index = 0;
                for (int j = last_found + 1; j < strlen(file_out); j++) {
                    tmp2[tmp2_index] = file_out[j];    
                }
                strncat(output, tmp2, strlen(tmp2));
            } else { // otherwise output should be ./file
                strcat(output, "./");
            }
            strcat(output, file_out);
            open(output, O_CREAT | O_WRONLY | O_TRUNC, S_IRWXU);
            free(output);
        } else if (exec_args[found_at + 1] == NULL) {
            print_error();
            exit(1);
        }
    }

    if (execv(PATH[found], exec_args) != 0) { 
//        for (int i = 0; i < in_path; i++) {
//           PATH[i] = NULL;
//        }
//        in_path = 0; 
        print_error();
        next_arg = " ";
        //printf("exec failed with path: %s and args: %s\n", PATH[found], *exec_args);
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

    // make path orginial without "cd"
    //strcpy(PATH[0], strtok(PATH[0], "cd"));
}

int in_path = 1;
void exec_path(char **exec_args) { // path
    in_path = 0;
    if (exec_args[1] != NULL) { // has args
        for (int j = 0; j < num_args - 1; j++) {
	        if (PATH[j] != NULL) 
		        free(PATH[j]);
	        PATH[j] = malloc(strlen(exec_args[j+1]) + 3);
	        char *tmp = malloc(strlen(exec_args[j+1]) + 3);
	        strcat(tmp, exec_args[j+1]);
	        strcpy(PATH[j], tmp);
	        strcat(PATH[j], "/");
	        free(tmp);
            in_path++;
	    }
    } else if (exec_args[1] == NULL) { // no args
        for (int k = 0; k < MAX_PATH - 1; k++) {
	        if (PATH[k] != NULL) 
		        free(PATH[k]);	
            PATH[k] = NULL;
        }
        in_path = 0;
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
    if (strcmp(exec_args[0], "cd") != 0 &&
        strcmp(exec_args[0], "path") != 0 &&
        strcmp(exec_args[0], "exit") != 0  
        ) {
        found = -1;
        for (int i = 0; i < MAX_PATH; i++) {
    	    if (PATH[i] != NULL && access(PATH[i], X_OK) == 0) {
	            found = i;
	        }
        }
    } 
    /*if (found == -1) {
        int ps = strlen(exec_args[0]) + strlen("/bin/");
        char *p = malloc(ps);
        for (int i = 0; i < in_path; i++) {
            PATH[i] = NULL;
        }
        PATH[0] = malloc(ps + 1);
        strcpy(PATH[0], strcat(p, exec_args[0]));
        found = 0;
        in_path = 1;
        free(p);
    }*/


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

