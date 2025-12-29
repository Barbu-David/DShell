#include <stdlib.h>
#include <string.h>

#include "read_write.h"
#include "launcher.h"
#include "commands.h"
#include "args.h"
#include "sf_wraps.h"
#include "parser.h"

void assign_executor(const char* c, Shell* dshell, Command* command)
{
  for (int i = 0; i < dshell->num_builtins; i++) 
    if (strcmp(c, dshell->builtin_str[i]) == 0) {
      command->execute = dshell->builtin_func[i];
      return;
    }
  command->execute =launch_task;
}

Command* parse(char** raw_args, Shell* dshell) 
{

  if (raw_args[0] == NULL) return 0;
  
  Command* command = init_command();

  assign_executor(raw_args[0], dshell, command);

  int j = 0;
  size_t count = 0;
  while (raw_args[count]) count++;
  char** tmp_args = init_args(count);

  for (int i = 0; raw_args[i]; i++) {

    if (strcmp(raw_args[i], "<") == 0) {
      if (!raw_args[i+1]) { print_error("Syntax error: no input file"); return NULL; }
      command->infile = sf_strdup(raw_args[++i]);
    }
    else if (strcmp(raw_args[i], ">") == 0) {
      if (!raw_args[i+1]) { print_error("Syntax error: no output file"); return NULL; }
      command->outfile = sf_strdup(raw_args[++i]);
    }
    else if (strcmp(raw_args[i], "&") == 0 && !raw_args[i+1]) {
      command->background = true;
    }
    else {
      tmp_args[j++] = raw_args[i];
    }
  }

  command->args = copy_args(tmp_args);

  free(raw_args);    
  free(tmp_args); 

  return command;
}
