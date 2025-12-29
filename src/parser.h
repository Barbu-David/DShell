#ifndef PARSER_H
#define PARSER_H

#include "dshell.h"
#include "commands.h"

#include "jobs.h"

Job* parse_job(char** raw_args, Shell* dshell);


void middle_parser(Command* command, int count, char** args);
void final_parser(Command* command, int count, char** args);
void only_parser(Command* command, int count, char** args);
void first_parser(Command* command, int count, char** args);
Command* build_command(char** raw_args, Shell* dshell, void (*parser) (Command*, int, char**));



#endif 
