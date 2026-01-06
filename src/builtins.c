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

  dshell->jobs[command->job_id]->history=false;

  if (!dshell->lastJob || dshell->lastJob->command_num == 0) {
    print_error("Empty history");
    return 0;
  }

  Job* job = init_job(dshell->lastJob->command_num);
  copy_job(dshell->lastJob, job);

  if (!job) {
    print_error("Failed to clone history");
    return 1;
  }

  add_job(dshell, job);

  return launch_job(job, dshell);
}


Builtin* init_builtins(int* num_builtins)
{
    static Builtin builtins_list[] = {
        {"help",    false, dsh_help},
        {"cd",      true,  dsh_cd},       
        {"banner",  false, dsh_banner},
        {"!!",      true, dsh_history},
        {"exit",    true,  dsh_exit}      
    };

    *num_builtins = sizeof(builtins_list) / sizeof(Builtin);

    return builtins_list;
}
