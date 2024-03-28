#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "batch.h"

#define MAX_INPUT 100

extern char error_message[30];

// interactive mode
int interactive() {
    while(1) {
        printf("wish> ");
        char input[MAX_INPUT];
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (input == NULL) {
            printf("Error reading input.\n");
            exit(1);
        } else {
            if (strcmp(input, "exit") == 0) {
                exit(0);
            } else {
                FILE *file = fopen("out", "w");
                if (file == NULL) {
                    printf("Error creating file.\n");
                    exit(1);
                }
                fprintf(file, "%s\n", input);
                fclose(file);
                batch("out");
                remove("out");
            }
        } 
    }
    return 0;
}

