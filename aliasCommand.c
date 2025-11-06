#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void alias_commands(char** cmd) {

    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided for aliasing.\n");
        return;
    }
    if(strcmp(cmd[0], "myps") == 0) {
        execvp("ps", (char* const[]){"ps", cmd[1], NULL});
        perror("execvp failed");
        exit(1);
    }
    else{
        execvp(cmd[0], cmd);
        perror("execvp failed");
        exit(1);
    }
}