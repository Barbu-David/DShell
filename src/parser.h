#ifndef PARSER_H
#define PARSER_H

#include "dshell.h"
#include "commands.h"

#include "jobs.h"

Job* parse_job(char** raw_args, Shell* dshell);

Command* parse(char** raw_args, Shell* dshell);

#endif 
