#ifndef BUILTINS_H
#define BUILTINS_H

#include "dshell.h" 

int dsh_help(char **args, Shell *shell);
int dsh_cd(char **args, Shell *shell);
int dsh_banner(char **args, Shell *shell);
int dsh_history(char **args, Shell *shell);
int dsh_exit(char **args, Shell *shell);

#endif

