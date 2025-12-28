#ifndef PARSER_H
#define PARSER_H

#include "dshell.h"
#include "commands.h"

Command* parse(char** raw_args, Shell* dshell);

#endif 
