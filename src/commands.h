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

Command* init_command();
void copy_command(Command* src, Command* dst);
void free_command(Command* c);

#endif 
