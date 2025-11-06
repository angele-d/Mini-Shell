#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

bool alias_commands(char** cmd) {

    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided\n");
        return false;
    }
    // myps alias for ps
    // TODO: Ask if it's what was asked
    else if(strcmp(cmd[0], "myps") == 0) {
        execvp("ps", (char* const[]){"ps", cmd[1], NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // mypstree alias for pstree -p
    else if(strcmp(cmd[0], "mypstree") == 0) {
        execvp("pstree", (char* const[]){"pstree", "-p", NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // mynetstat alias for netstat -tunap
    // FIXME: Ask professor if it's normal
    else if(strcmp(cmd[0], "mynetstat") == 0) {
        printf("---\nMYENV INFO > WARNING: this command may not work if net-tools is not installed\n---\n");
        execvp("netstat", (char* const[]){"netstat", "-tunap", NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // myarp alias for arp -n
    // FIXME: Ask professor if it's normal
    else if(strcmp(cmd[0], "myarp") == 0) {
        printf("---\nMYENV INFO > arp -n is deprecated, using ip neigh show instead.\n---\n");
        execvp("ip", (char* const[]){"ip", "neigh", "show", NULL});
        perror("execvp failed");
        exit(1);
        return true;
    }
    // myexe
    // XXX: implement myexe command when information about it provided
    else if(strcmp(cmd[0], "myexe") == 0) {
        printf("---\nMYENV INFO > myexe command not implemented yet.\n---\n");
        exit(1);
        return true;
    }
    return false;
}

// TODO: Implement home_made_commands
bool home_made_commands(char** cmd) {
    // Placeholder for additional custom commands
    // Currently, it just calls alias_commands
    return false;
}

void commands(char** cmd) {

    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided\n");
        return;
    }
    // Try alias commands first
    if (!alias_commands(cmd)) {
        printf("---\n MYENV INFO > Command not found, trying home made commands...\n---\n");
        // Then try home made commands
        if(!home_made_commands(cmd)) {
            printf("---\n MYENV INFO > Command not found, classical command execution...\n---\n");
            // Then do classical command
            execvp(cmd[0], cmd);
            perror("execvp failed");
            exit(1);
            return;
        }
    }
}