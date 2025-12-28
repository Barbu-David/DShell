#ifndef COMMANDS_H
#define COMMANDS_H

#include "dshell.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Command{
    char** args;              
    char* infile;              
    char* outfile;            
    bool background;
    bool to_history;
    int (*execute)(struct Command*, Shell*);
} Command;

Command* command_init(size_t size);
void free_command(Command* c);
void copy_command(Command* src, Command* dst);
void free_command_deep(Command* c);

#endif 
