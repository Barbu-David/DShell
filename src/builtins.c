#include "builtins.h"
#include "ui.h"
#include "read_write.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dsh_execute.h"

int dsh_banner(char** args, Shell* dshell)
{
  (void) args;
  (void) dshell;
  print_banner(PURPLE);
  return 0;
}

int dsh_cd(char** args, Shell* dshell)
{  
  (void) dshell;
  if (args[1] == NULL) print_error("lsh: expected argument to \"cd\"\n");
  else if (chdir(args[1]) != 0) print_error("chdir failed");
  return 0;
}

int dsh_help(char** args, Shell* dshell) 
{
  (void) dshell;
  (void) args;
  printf("Hello, this is dshell. It has the following builtins:\n");
  for(int i=0; i< dshell->num_builtins; i++) printf("%s\n", dshell->builtin_str[i]);
  printf("Type program names and arguments, and hit enter.\n");
  return 0;

}

int dsh_exit(char** args, Shell* dshell)
{
  (void) args;
  dshell->running=false;
  return 0;
}

int dsh_history(char ** args, Shell* dshell) 
{
  (void)args;

  if(dshell->history_args==NULL) {
    print_error("Empty history");
    return 1;
  }

  return dsh_execute(dshell->history_args, dshell);
}

