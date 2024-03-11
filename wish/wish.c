#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include "batch.h"
#include "interactive.h"
#define MAX_INPUT 100
#define MAX_ARGS 20

int lineNum;
char input[MAX_INPUT];
char error_message[30] = "An error has occurred\n";

int main(int argc, char *argv[]) {
    lineNum = 1;
    char *filename = argv[1];
    int rc = fork();
    if (rc < 0) {
	write(STDERR_FILENO, error_message, sizeof(error_message));
	exit(1);
    } else if (rc == 0) {
        if (argc < 2){
            interactive();
        } else {	
            batch(filename);
        }
    } else {
        wait(NULL);
    }
    
    return 0;
}

