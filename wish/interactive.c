#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX_INPUT 100

extern int lineNum;
extern char input[MAX_INPUT];

// interactive mode
int interactive() {
    while(1) {
        printf("wish> ");
        if (fgets(input, MAX_INPUT, stdin) == NULL) {
            printf("Error reading input.\n");
            exit(1);
        }

	// exit
	if (strcmp(input, "exit") == 0)
            exit(0);

    }
    return 0;
}

