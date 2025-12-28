#include "builtins.h"
#include "ui.h"
#include "read_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int dsh_banner(Command* command, Shell* dshell)
{
  (void) command;
  (void) dshell;
  print_banner(PURPLE);
  return 0;
}

int dsh_cd(Command* command, Shell* dshell)
{  
  (void) dshell;
  if (command->args[1] == NULL) print_error("lsh: expected argument to \"cd\"\n");
  else if (chdir(command->args[1]) != 0) print_error("chdir failed");
  return 0;
}

int dsh_help(Command* command, Shell* dshell) 
{
  (void) dshell;
  (void) command;
  printf("Hello, this is dshell. It has the following builtins:\n");
  for(int i=0; i< dshell->num_builtins; i++) printf("%s\n", dshell->builtin_str[i]);
  printf("Type program names and arguments, and hit enter.\n");
  return 0;

}

int dsh_exit(Command* command, Shell* dshell)
{
  (void) command;
  dshell->running=false;
  return 0;
}

int dsh_history(Command* command, Shell* dshell) 
{
  (void)command;

  if(dshell->history_args==NULL) {
    print_error("Empty history");
    return 1;
  }

  return dshell->historyCommand->execute(dshell->historyCommand, dshell);
}

