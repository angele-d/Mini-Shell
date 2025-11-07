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
    // FIXME: Ask if it's what was asked
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

void command_myenv(int PID){
    char chemin[64];
    sprintf(chemin, "/proc/%d/environ", PID);
    FILE* file = fopen(chemin, "r");
    if (file == NULL) {
         perror("fopen failed");
         return;
    }
    char line[256];
    while (fgets(line, sizeof(line), file) != NULL) {
        for (char* p = line; *p != '\0'; p++) {
            if (*p == '\0') {
                *p = '\n';
            }
        }
        // Print the modified line
        printf("%s\n", line);
    }
    fclose(file);
}


bool home_made_commands(char** cmd) {
    // XXX: implement myinfo command when information about it provided
    // myinfo implementation
    if(strcmp(cmd[0], "myinfo") == 0) {
        printf("---\nMYENV INFO > myinfo command not implemented yet.\n---\n");
        return true;
    }
    // myenv implementation
    else if(strcmp(cmd[0], "myenv") == 0) {
        // if PID provided
        if(strcmp(cmd[1], "-p") == 0 && cmd[2] != NULL) {
            int pid = atoi(cmd[2]);
            command_myenv(pid);
        }
        // else current process
        else {
            command_myenv(getpid());
        }
        return true;
    }
    // XXX: implement mymaps command when information about it provided
    // mymaps implementation
    else if(strcmp(cmd[0], "mymaps") == 0) {
        printf("---\nMYENV INFO > mymaps command not implemented yet.\n---\n");
        return true;
    }
    // XXX: implement mydump command when information about it provided
    // mydump implementation
    else if(strcmp(cmd[0], "mydump") == 0) {
        printf("---\nMYENV INFO > mydump command not implemented yet.\n---\n");
        return true;
    }
    return false;
}

void commands(char** cmd) {
    if(cmd[0] == NULL) {
        fprintf(stderr, "No command provided\n");
        return;
    }
    // Try alias commands first
    if (!alias_commands(cmd)) {
        // Then try home made commands
        if(!home_made_commands(cmd)) {
            // Then do classical commands
            execvp(cmd[0], cmd);
            perror("execvp failed");
            exit(1);
        }
    }
}