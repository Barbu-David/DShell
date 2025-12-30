#include "commands.h"
#include "args.h"
#include "sf_wraps.h"
#include "read_write.h"
#include "jobs.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>    
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include "builtins.h"

void free_command(Command* c)
{
  free_args(c->args);
  free(c);
}

Command* init_command()
{

  Command* command = (Command*) sf_malloc(sizeof(Command));
  command->job_id=0;
  command->execute = NULL;
  command->in_fd=-1;
  command->out_fd=-1;
  command->parent_only = false;
  command->args = NULL;

  return command;
}

void copy_command(Command* src, Command* dst)
{
  if (!src || !dst) return;

  if (dst->args) {
    free_args(dst->args);
    dst->args = NULL;
  }

  dst->parent_only  = src->parent_only;
  dst->execute     = src->execute;

  dst->in_fd = src->in_fd;
  dst->out_fd = src->out_fd;

  dst->args = copy_args(src->args);
}

int external_cmd_execute(Command* command, Shell* dshell)
{
  (void) dshell;
  if (execvp(command->args[0], command->args) == -1) {
    print_error(strerror(errno));
    print_error("Failed to start program");
  }
  return -1;
}

void child(Command* command, Shell* dshell) 
{
  if (!command || !command->args || !command->args[0]) {
    print_error("Invalid arguments");
    _exit(127);
  }

  if (command->in_fd != -1) {
    if (dup2(command->in_fd, STDIN_FILENO) == -1) {
      print_error("dup2 error for stdin");
      print_error(strerror(errno));
      _exit(127);
    }
    close(command->in_fd);
    command->in_fd = -1;
  }

  if (command->out_fd != -1) {
    if (dup2(command->out_fd, STDOUT_FILENO) == -1) {
      print_error("dup2 error for stdout");
      print_error(strerror(errno));
      _exit(127);
    }
    close(command->out_fd);
    command->out_fd = -1;
  }

  command->execute(command, dshell);

     int ret = command->execute(command, dshell);
    _exit(ret >= 0 ? ret : 127);
}

void launch_command(Command* command, Shell* dshell)
{

  if (command->parent_only) {
    command->execute(command, dshell);
    return;
  }

  pid_t pid;

  pid = fork();

  if(pid<0) {
    print_error("Fork failed");
    print_error(strerror(errno));
  }

  if(pid==0) child(command, dshell); 

  if(!dshell->jobs[command->job_id]->pgid) dshell->jobs[command->job_id]->pgid = pid;

  setpgid(pid, dshell->jobs[command->job_id]->pgid);

}

void assign_executor(const char* c, Shell* dshell, Command* command)
{
  for (int i = 0; i < dshell->num_builtins; i++) 
    if (strcmp(c, dshell->builtins[i].str) == 0) {
      command->execute = dshell->builtins[i].func;
      command->parent_only=dshell->builtins[i].parent_only;
      return;
    }
  command->execute =external_cmd_execute;
}

Command* build_command(char** raw_args, Shell* dshell, void (*parser) (Command*, int, char**)) 
{

  if (raw_args[0] == NULL || parser == NULL) return 0;

  Command* command = init_command();

  assign_executor(raw_args[0], dshell, command);

  size_t count = 0;
  while (raw_args[count]) count++;

  parser(command, count, raw_args);

  return command;
}
