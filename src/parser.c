#include "parser.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "read_write.h"
#include "launcher.h"

Command* parse(char** raw_args, Shell* dshell) 
{
  
  if (raw_args[0] == NULL) return 0;
  
  Command* command = malloc(sizeof(Command));
  
  command->args = malloc(sizeof(raw_args));

  command->args[0] = raw_args[0];

  bool builtin = false;

  for (int i = 0; i < dshell->num_builtins; i++) 
    if (strcmp(command->args[0], dshell->builtin_str[i]) == 0) {
      command->execute = dshell->builtin_func[i];
      command->to_history = (i == dshell->history_num) ? true:false;
      builtin = true;
    }

  command->infile = NULL;
  command->outfile = NULL;
  command->background = false;

  int j = 0;
 
  for (int i = 0; raw_args[i]; i++) {

    if (strcmp(raw_args[i], "<") == 0) {
      if (!raw_args[i+1]) { print_error("Syntax error: no input file"); return NULL; }
      command->infile = raw_args[++i];
    }
    else if (strcmp(raw_args[i], ">") == 0) {
      if (!raw_args[i+1]) { print_error("Syntax error: no output file"); return NULL; }
      command->outfile = raw_args[++i];
    }
    else if (strcmp(raw_args[i], "&") == 0 && !raw_args[i+1]) {
      command->background = true;
    }
    else {
      command->args[j++] = raw_args[i];
    }
  }
  command->args[j] = NULL;

  if(!builtin) command->execute = launch_task;

  return command;
}
