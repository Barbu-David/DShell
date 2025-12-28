#include <stdlib.h>
#include <string.h>

#include <stdbool.h>
#include "dshell.h"
#include "ui.h"
#include "read_write.h"
#include "line_tokenizer.h"
#include "builtins.h"
#include "args.h"
#include "background.h"
#include "parser.h"
#include "commands.h"

#include <stdio.h>

Shell* shell_init() {

    print_banner(PURPLE);

    Shell* dshell = malloc(sizeof(Shell));
    if (!dshell) {
        print_error("malloc failed");
        exit(1);
    }

    dshell->historyCommand = command_init(BUFSIZ);
    dshell->running = true;
    dshell->background_process = 0;

    static char *builtins[] = {"help", "cd", "banner", "!!", "exit"};
    static int (*funcs[])(Command* command, struct Shell *shell) = {
        dsh_help,
        dsh_cd,
        dsh_banner,
        dsh_history,
        dsh_exit
    };
    
    dshell->num_builtins = 5;
    dshell->builtin_str = builtins;
    dshell->builtin_func = funcs;

    return dshell;
}

void shell_close(Shell* dshell) 
{
  free_command_deep(dshell->historyCommand);
  free(dshell);
}

void shell_step(Shell* dshell) 
{
  print_shell_prompt();

  char* line = read_line();
  char** args = tokenize_line(line);
  Command* command = parse(args, dshell);
  
  if (!command) {
        free_args(args);  
        free(line);
        return;
  }
  
  int status = command->execute(command, dshell);

  if(status == -1) print_error("Failed to execute program");

  if(dshell->background_process) reap_background_process(dshell); 

  if(command->to_history) copy_command(command, dshell->historyCommand);
  
  free_command(command);
  free(line);

}
