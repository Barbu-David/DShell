#ifndef COMMANDS_H
#define COMMANDS_H

#include "dshell.h"
#include <stdbool.h>
#include <stddef.h>

typedef struct Command{
    char** args;              
    int in_fd;              
    int out_fd;  
    int job_id;
    bool parent_only;
    int (*execute)(struct Command*, Shell*);
} Command;

Command* init_command();
Command* build_command(char** raw_args, Shell* dshell, void (*parser) (Command*, int, char**));
void launch_command(Command* command, Shell* dshell);
void copy_command(Command* src, Command* dst);
void free_command(Command* c);

#endif 
