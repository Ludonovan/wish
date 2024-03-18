#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batch.h"
#define MAX_INPUT 100

extern int lineNum;
char input[MAX_INPUT];  

// interactive mode
int interactive() {
    while(1) {
        printf("wish> ");
        scanf("%s" , input);
        if (input == NULL) {
            printf("Error reading input.\n");
            exit(1);
        } else {
	        // exit
	        if (strcmp(input, "exit") == 0 ) {
                exit(0);
            } else {
               // TODO create file, put input into file, clear file after running 
            }
        } 
    }
    return 0;
}

