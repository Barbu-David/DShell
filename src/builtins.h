#ifndef BUILTINS_H
#define BUILTINS_H

#include "dshell.h" 
#include "parser.h"

typedef struct Builtin {
  
  char* str;
  bool parent_only;
  int (*func)(Command* command, struct Shell *shell);

} Builtin;

int dsh_help(Command* command, Shell *shell);
int dsh_cd(Command* command, Shell *shell);
int dsh_banner(Command* command, Shell *shell);
int dsh_history(Command* command, Shell *shell);
int dsh_exit(Command* command, Shell *shell);

Builtin* init_builtins(int* num_builtins);

#endif

