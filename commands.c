#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void alias_commands(char** cmd) {

    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided\n");
        return;
    }
    // myps alias for ps
    // TODO: Ask if it's what was asked
    else if(strcmp(cmd[0], "myps") == 0) {
        execvp("ps", (char* const[]){"ps", cmd[1], NULL});
        perror("execvp failed");
        exit(1);
    }
    // mypstree alias for pstree -p
    else if(strcmp(cmd[0], "mypstree") == 0) {
        execvp("pstree", (char* const[]){"pstree", "-p", NULL});
        perror("execvp failed");
        exit(1);
    }
    // mynetstat alias for netstat -tunap
    // FIXME: Ask professor if it's normal
    else if(strcmp(cmd[0], "mynetstat") == 0) {
        printf("---\nMYENV INFO > WARNING: this command may not work if net-tools is not installed\n---\n");
        execvp("netstat", (char* const[]){"netstat", "-tunap", NULL});
        perror("execvp failed");
        exit(1);
    }
    // myarp alias for arp -n
    // FIXME: Ask professor if it's normal
    else if(strcmp(cmd[0], "myarp") == 0) {
        printf("---\nMYENV INFO > arp -n is deprecated, using ip neigh show instead.\n---\n");
        execvp("ip", (char* const[]){"ip", "neigh", "show", NULL});
        perror("execvp failed");
        exit(1);
    }
    // myexe
    // TODO: implement myexe command when information about it provided
    else if(strcmp(cmd[0], "myexe") == 0) {
        printf("---\nMYENV INFO > myexe command not implemented yet.\n---\n");
        exit(1);
    }
    // classical command
    else{
        execvp(cmd[0], cmd);
        perror("execvp failed");
        exit(1);
    }
}