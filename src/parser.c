#define _POSIX_C_SOURCE 200809L
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#include "read_write.h"
#include "launcher.h"
#include "commands.h"
#include "args.h"
#include "parser.h"

Command* parse(char** raw_args, Shell* dshell) 
{
  
  if (raw_args[0] == NULL) return 0;
  
  size_t count = 0;
  while (raw_args[count]) count++;

  Command* command = init_command();

  bool builtin = false;

  for (int i = 0; i < dshell->num_builtins; i++) 
    if (strcmp(raw_args[0], dshell->builtin_str[i]) == 0) {
      command->execute = dshell->builtin_func[i];
      builtin = true;
    }

  char** tmp_args = init_args(count);
  int j = 0;
 
  for (int i = 0; raw_args[i]; i++) {

    if (strcmp(raw_args[i], "<") == 0) {
      if (!raw_args[i+1]) { print_error("Syntax error: no input file"); return NULL; }
      command->infile = strdup(raw_args[++i]);
    }
    else if (strcmp(raw_args[i], ">") == 0) {
      if (!raw_args[i+1]) { print_error("Syntax error: no output file"); return NULL; }
      command->outfile = strdup(raw_args[++i]);
    }
    else if (strcmp(raw_args[i], "&") == 0 && !raw_args[i+1]) {
      command->background = true;
    }
    else {
      tmp_args[j++] = raw_args[i];
    }

  }

  if(!builtin) command->execute = launch_task;
 
  command->args = copy_args(tmp_args);

  free(raw_args);    
  free(tmp_args); 

  return command;
}
