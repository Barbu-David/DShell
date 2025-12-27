#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include "dshell.h"
#include "ui.h"
#include "read_write.h"
#include "line_tokenizer.h"
#include "dsh_execute.h"
#include "builtins.h"
#include "args.h"
#include "background.h"

Shell* shell_init() {
    print_banner(PURPLE);

    Shell* dshell = malloc(sizeof(Shell));
    if (!dshell) {
        print_error("malloc failed");
        exit(1);
    }

    dshell->running = true;
    dshell->background_process = 0;
    dshell->history_args = NULL;

    static char *builtins[] = {"help", "cd", "banner", "!!", "exit"};
    static int (*funcs[])(char **args, struct Shell *shell) = {
        dsh_help,
        dsh_cd,
        dsh_banner,
        dsh_history,
        dsh_exit
    };
    
    dshell->history_num= 3;
    dshell->num_builtins = 5;
    dshell->builtin_str = builtins;
    dshell->builtin_func = funcs;

    return dshell;
}

void shell_close(Shell* dshell) 
{
  free_args(dshell->history_args);
  free(dshell);
}

void shell_step(Shell* dshell) 
{
  print_shell_prompt();

  char* line = read_line();
  char** args = tokenize_line(line);
  int status = dsh_execute(args, dshell);

  if(status == -1) print_error("Failed to execute program");

  if(dshell->background_process) reap_background_process(dshell); 

  free(args);
  free(line);

}
