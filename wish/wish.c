#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "batch.h"
#include "interactive.h"
#define MAX_ARGS 20

int lineNum;
char error_message[30] = "An error has occurred\n";

int main(int argc, char *argv[]) {
    lineNum = 1;
    char *filename = argv[1];
    if (argc < 2) {
         interactive();
    } else if (argc == 2) {	
        batch(filename);
    } else {
       write(STDERR_FILENO, error_message, sizeof(error_message));
       exit(1);
    } 
    return 0;
}

