#ifndef DSHELL_H
#define DSHELL_H

#include <stdbool.h>

struct Command;
typedef struct Command Command;

typedef struct Shell {
    bool running;
    int background_process;
    Command* historyCommand;  

    int num_builtins;
    char **builtin_str;
    int (**builtin_func)(Command* command, struct Shell *shell); 
} Shell;

Shell* shell_init();
void shell_step(Shell* dshell);
void shell_close(Shell* dshell);

#endif 

