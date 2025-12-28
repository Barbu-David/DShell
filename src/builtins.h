#ifndef BUILTINS_H
#define BUILTINS_H

#include "dshell.h" 
#include "parser.h"

int dsh_help(Command* command, Shell *shell);
int dsh_cd(Command* command, Shell *shell);
int dsh_banner(Command* command, Shell *shell);
int dsh_history(Command* command, Shell *shell);
int dsh_exit(Command* command, Shell *shell);

#endif

