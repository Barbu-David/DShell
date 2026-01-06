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


int external_cmd_execute(Command* command, Shell* dshell)
{
    (void) dshell;
    execvp(command->args[0], command->args);

    // Only reached if execvp failed:
    print_error(strerror(errno));
    print_error("Failed to start program external_cmd_execute");
    return 127;   // conventional exit code for "command not found / exec failed"
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

  dst->in_fd = -1;
  dst->out_fd = -1;


  dst->args = copy_args(src->args);

  dst->execute = src->execute;
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

  int ret = command->execute(command, dshell);
  _exit(ret);
}

pid_t launch_command(Command* command, Shell* dshell, int (*pipe_fds)[2], int num_pipes)
{
    if (command->parent_only) {
        command->execute(command, dshell);
        return 0;
    }

    pid_t pid = fork();
    if (pid < 0) {
        print_error("Fork failed");
        print_error(strerror(errno));
        return -1;
    }

    if (pid == 0) {
        /* CHILD */
        /* Put child in its own process group (typical pattern) */
        if (setpgid(0, 0) < 0) {
            /* non-fatal: we still try to run the child */
            print_error("child: setpgid(0,0) failed");
            print_error(strerror(errno));
        }

        /* Close *all* pipe fds that this child does not need.
           pipe_fds may be NULL if num_pipes == 0 (single command). */
        if (pipe_fds != NULL && num_pipes > 0) {
            for (int p = 0; p < num_pipes; ++p) {
                int rfd = pipe_fds[p][0];
                int wfd = pipe_fds[p][1];

                if (rfd != -1 && rfd != command->in_fd) {
                    close(rfd);
                }
                if (wfd != -1 && wfd != command->out_fd) {
                    close(wfd);
                }
            }
        }

        /* Now let child() perform dup2 of its in/out fds and exec */
        child(command, dshell);
        _exit(127); /* should not be reached */
    }

    /* PARENT */
    return pid;
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
