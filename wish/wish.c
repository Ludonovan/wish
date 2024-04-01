#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "batch.h"
#include "interactive.h"
#include "exec.h"
#define MAX_ARGS 20

int lineNum;
char error_message[30] = "An error has occurred\n";

int main(int argc, char *argv[]) {
    lineNum = 1;

    if (argc < 2) {
        while (1) {
            interactive();
        }
    } else if (argc == 2) {
        if (access(argv[1], F_OK) == 0) {
            batch(argv[1]);
        } else {
            print_error();
            exit(1);
        }
    } else {
        print_error();
        exit(1);
    } 
    return 0;
}

