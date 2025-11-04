#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MAX_LINE_LENGTH 80
#define MAX_ARGS 20

void parse_command(char* command, char** cmd_args) { /*
    Parse a single command into its arguments
    */
    char* token;
    int index = 0;

    token = strtok(command, " \n\t");
    while (token != NULL && index < MAX_ARGS - 1) {
        cmd_args[index] = token;
        index++;
        token = strtok(NULL, " \n\t");
    }
    cmd_args[index] = NULL;
}

void parse_line(char* line, char** args) { /*
    Parse the input line into commands separated by |, <, >
    */
    char* token;
    int index = 0;

    token = strtok(line, "|<>");
    while (token != NULL && index < MAX_ARGS - 1) {
        args[index] = token;
        index++;
        token = strtok(NULL, "|<>");
    }
    args[index] = NULL;
}

void pipe_commands(char* init_line, char** init_args, char** cmd1, char** cmd2) {
    //Parse the input line
    parse_line(init_line, init_args);
    if(init_args[0] == NULL){ // No command entered
        return;
    }

    //Parse first command
    parse_command(init_args[0], cmd1);
    if(strcmp(cmd1[0], "exit") == 0){ //Exit command
        printf("Closing myenv...\n");
        return;
    }
    
    //Parse second command
    parse_command(init_args[1], cmd2);
    if(cmd2[0] == NULL){ // No second command specified
        fprintf(stderr, "No command registered after |\n");
        return;
    }
    
    int fd[2];
    pipe(fd);

    //Execute first command
    pid_t pid1 = fork();
    if (pid1 < 0) { perror("Fork failed"); return;}
    if (pid1 == 0) { //Child process
        dup2(fd[1], 1); //Redirect stdout to second command if exists
        close(fd[1]);
        close(fd[0]);
        execvp(cmd1[0], cmd1);
        perror("execvp failed");
        exit(1);
    }

    //Execute second command
    pid_t pid2 = fork();
    if (pid2 < 0) { perror("Fork failed"); return;}
    if (pid2 == 0) { //Child process
        dup2(fd[0], 0); //Redirect stdin to first command
        close(fd[0]);
        close(fd[1]);
        execvp(cmd2[0], cmd2);
        perror("execvp failed");
        exit(1);
    }

    //Closing
    close(fd[0]);
    close(fd[1]);
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
}

void input_commands(char* init_line, char** init_args, char** cmd1, char** cmd2) {
    //Parse the input line
    parse_line(init_line, init_args);
    if(init_args[0] == NULL){ // No command entered
        return;
    }

    //Parse first command
    parse_command(init_args[0], cmd1);
    if(strcmp(cmd1[0], "exit") == 0){ //Exit command
        printf("Closing myenv...\n");
        return;
    }

    //Parse second command
    parse_command(init_args[1], cmd2);
    if(cmd2[0] == NULL){ // No file specified
        fprintf(stderr, "No file registered after <\n");
        return;
    }
    
    int file;
    file = open(cmd2[0], O_RDONLY);
    if (file < 0) {
        perror("Failed to open file");
        return;
    }

    //Execute first command
    pid_t pid1 = fork();
    if (pid1 < 0) { perror("Fork failed"); return;}
    if (pid1 == 0) { //Child process
        dup2(file, 0); //Redirect stdin to file
        close(file);
        execvp(cmd1[0], cmd1);
        perror("execvp failed");
        exit(1);
    }
    close(file);
    waitpid(pid1, NULL, 0);
}

void output_commands(char* init_line, char** init_args, char** cmd1, char** cmd2) {
    //Parse the input line
    parse_line(init_line, init_args);
    if(init_args[0] == NULL){ // No command entered
        return;
    }

    //Parse first command
    parse_command(init_args[0], cmd1);
    if(strcmp(cmd1[0], "exit") == 0){ //Exit command
        printf("Closing myenv...\n");
        return;
    }

    //Parse second command
    parse_command(init_args[1], cmd2);
    if(cmd2[0] == NULL){ // No file specified
        fprintf(stderr, "No file registered after >\n");
        return;
    }
    
    int file;
    file = open(cmd2[0], O_CREAT | O_WRONLY | O_TRUNC, 0644); //0644 = rw-r--r--
    if (file < 0) {
        perror("Failed to open file");
        return;
    }

    //Execute first command
    pid_t pid1 = fork();
    if (pid1 < 0) { perror("Fork failed"); return;}
    if (pid1 == 0) { //Child process
        dup2(file, 1); //Redirect stdout to file
        close(file);
        execvp(cmd1[0], cmd1);
        perror("execvp failed");
        exit(1);
    }
    close(file);
    waitpid(pid1, NULL, 0);
}

int main(){
    char init_line[MAX_LINE_LENGTH];
    char* init_args[3]; // To hold up to two commands and NULL
    char* cmd1[MAX_ARGS];
    char* cmd2[MAX_ARGS];

    while(1){
        printf("myenv > ");
        fflush(stdout);

        //Read input line
        if (fgets(init_line, sizeof(init_line), stdin) == NULL) {
            printf("Closing myenv...\n");
            break; // EOF OR CTRL+D
        }

        if(strchr(init_line, '|')){ // Handle pipe
            pipe_commands(init_line, init_args, cmd1, cmd2);
        }
        else if(strchr(init_line, '<')){ // Handle redirection input
            input_commands(init_line, init_args, cmd1, cmd2);
        }
        else if(strchr(init_line, '>')){ // Handle redirection output
            output_commands(init_line, init_args, cmd1, cmd2);
        }
        else{ // Single command
            //Parse the input line
            parse_line(init_line, init_args);
            if(init_args[0] == NULL){ // No command entered
                continue;
            }

            //Parse first command
            parse_command(init_args[0], cmd1);
            if(strcmp(cmd1[0], "exit") == 0){ //Exit command
                printf("Closing myenv...\n");
                break;
            }

            pid_t pid = fork();
            if (pid < 0) { perror("Fork failed"); continue;}
            if (pid == 0) { //Child process
                execvp(cmd1[0], cmd1);
                perror("execvp failed");
                exit(1);
            }
            waitpid(pid, NULL, 0);
            continue;
        }

        
    }
    return 0;
}
