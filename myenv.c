#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "commands.h"

#define MAX_LINE_LENGTH 80
#define MAX_ARGS 20

void parse_command(char* command, char** cmd_args) { /*
    Parse a single command into its arguments, handling quoted strings
    */
    int index = 0;
    char* ptr = command;
    char* start;
    
    // Skip leading whitespace
    while (*ptr && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')) {
        ptr++;
    }
    
    while (*ptr && index < MAX_ARGS - 1) {
        start = ptr;
        
        // Check if we're starting a quoted string
        if (*ptr == '"' || *ptr == '\'') {
            char quote_char = *ptr;
            ptr++; // Skip opening quote
            start = ptr;
            
            // Find the closing quote
            while (*ptr && *ptr != quote_char) {
                ptr++;
            }
            
            if (*ptr == quote_char) {
                *ptr = '\0'; // Null-terminate the string (removing closing quote)
                cmd_args[index++] = start;
                ptr++; // Move past the closing quote
            }
        } else {
            // Regular token - read until whitespace
            while (*ptr && *ptr != ' ' && *ptr != '\t' && *ptr != '\n') {
                ptr++;
            }
            
            if (*ptr) {
                *ptr = '\0'; // Null-terminate
                ptr++;
            }
            
            cmd_args[index++] = start;
        }
        
        // Skip trailing whitespace
        while (*ptr && (*ptr == ' ' || *ptr == '\t' || *ptr == '\n')) {
            ptr++;
        }
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

int count_pipes(const char *line) {
    int count = 0;
    while (*line) {
        if (*line == '|')
            count++;
        line++;
    }
    return count;
}

void pipe_commands(char* init_line, char** init_args, char** cmd1, char** cmd2) {
    if (count_pipes(init_line) > 1) {
        fprintf(stderr, "Error: More than one pipe is not supported.\n");
        return;
    }
    
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
        commands(cmd1);
        exit(0);
    }

    //Execute second command
    pid_t pid2 = fork();
    if (pid2 < 0) { perror("Fork failed"); return;}
    if (pid2 == 0) { //Child process
        dup2(fd[0], 0); //Redirect stdin to first command
        close(fd[0]);
        close(fd[1]);
        commands(cmd2);
        exit(0);
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
        commands(cmd1);
        exit(0);
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
        commands(cmd1);
        exit(0);
    }
    close(file);
    waitpid(pid1, NULL, 0);
}

int main(void){
    char init_line[MAX_LINE_LENGTH];
    char* init_args[3]; // To hold up to two commands and NULL
    char* cmd1[MAX_ARGS];
    char* cmd2[MAX_ARGS];

    while(1){
        fprintf(stderr,"\033[34mmyenv >\033[0m ");
        fflush(stdout);

        //Read input line
        if (fgets(init_line, sizeof(init_line), stdin) == NULL) {
            printf("\033[34mClosing myenv...\033[0m\n");
            break; // EOF OR CTRL+D
        }
        if(strchr(init_line, '|')){ // Handle pipe
            pipe_commands(init_line, init_args, cmd1, cmd2);
        }
        else if (strchr(init_line, '<') && strchr(init_line, '>')) {

            char *cmd_part;
            char *infile;
            char *outfile;

            cmd_part = strtok(init_line, "<>");
            infile   = strtok(NULL, "<>");
            outfile  = strtok(NULL, "<>");

            if (!cmd_part || !infile || !outfile) {
                fprintf(stderr, "Syntax error\n");
                continue;
            }

            infile = strtok(infile, " \t\n");
            outfile = strtok(outfile, " \t\n");

            parse_command(cmd_part, cmd1);

            int fd_in = open(infile, O_RDONLY);
            if (fd_in < 0) {
                perror("open input");
                continue;
            }

            int fd_out = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd_out < 0) {
                perror("open output");
                close(fd_in);
                continue;
            }

            pid_t pid = fork();
            if (pid == 0) {
                dup2(fd_in, STDIN_FILENO);
                dup2(fd_out, STDOUT_FILENO);

                close(fd_in);
                close(fd_out);

                commands(cmd1);
                exit(0);
            }

            close(fd_in);
            close(fd_out);
            waitpid(pid, NULL, 0);
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
                printf("\033[34mClosing myenv...\033[0m\n");
                break;
            }

            pid_t pid = fork();
            if (pid < 0) { perror("Fork failed"); continue;}
            if (pid == 0) { //Child process
                commands(cmd1);
            }
            waitpid(pid, NULL, 0);
            continue;
        }

    }
    return 0;
}
