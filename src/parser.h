#ifndef PARSER_H
#define PARSER_H

#include "dshell.h"

typedef struct Command{
    char** args;              
    char* infile;              
    char* outfile;            
    bool background;
    bool to_history;
    int (*execute)(struct Command*, Shell*);
} Command;

Command* parse(char** raw_args, Shell* dshell);

#endif 
