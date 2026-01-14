#ifndef COMMANDS_H
#define COMMANDS_H
#include <stdbool.h>

//Non-called functions declarations
void command_myenv(int pid);
void command_myexe(char* chemin);
bool alias_commands(char** cmd);
bool home_made_commands(char** cmd);

//Called functions declarations
void commands(char** cmd);

#endif // COMMANDS_H