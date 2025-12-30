#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "builtins.h"
#include "ui.h"
#include "read_write.h"


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
  if (command->args[1] == NULL) print_error("expected arguments to cd");
  else if (chdir(command->args[1]) != 0) {
    print_error("chdir failed");
    print_error(strerror(errno));
    return 1;
  }
  return 0;
}

int dsh_help(Command* command, Shell* dshell) 
{
  (void) dshell;
  (void) command;
  printf("Hello, this is dshell. It has the following builtins:\n");
  for(int i=0; i< dshell->num_builtins; i++) printf("%s\n", dshell->builtins[i].str);
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

  (void) command;

  if (!dshell->historyCommand ||
      !dshell->historyCommand->args ||
      dshell->historyCommand->args[0] == NULL ||
      dshell->historyCommand->execute == NULL) {
    print_error("Empty history");
    return 0;
  }

  return dshell->historyCommand->execute(dshell->historyCommand, dshell);
}

Builtin* init_builtins(int* num_builtins)
{
    static Builtin builtins_list[] = {
        {"help",    false, dsh_help},
        {"cd",      true,  dsh_cd},       // parent only
        {"banner",  false, dsh_banner},
        {"!!",      false, dsh_history},
        {"exit",    true,  dsh_exit}      // parent only
    };

    *num_builtins = sizeof(builtins_list) / sizeof(Builtin);

    return builtins_list;
}
