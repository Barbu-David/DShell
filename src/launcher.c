#include "launcher.h"
#include "read_write.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include "parser.h"
#include "dshell.h"

int launch_task(Command* command, Shell* dshell)
{
  pid_t pid;

  pid = fork();

  if(pid<0) {
    print_error("Fork failed");
    return -1;
  }

  if(pid==0) {

    if(!(command->args) || !(command->args[0])) {
      print_error("Invalid arguments");
      _exit(127);
    }
         if (command->background) {
         setpgid(0, 0); // new process group for background
         }
        if (command->infile) {
            int fd = open(command->infile, O_RDONLY);
            if (fd == -1) { print_error("open"); _exit(127); }
            dup2(fd, STDIN_FILENO); //TO DO add error checking
            close(fd);
        }

        if (command->outfile) {
            int fd = open(command->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) { print_error("open"); _exit(127); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

    if(execvp(command->args[0], command->args) == -1) {
      print_error("Failed to start program");
      _exit(127);
    }
  }


  if(command->to_history) dshell->historyCommand = command;
  //else free command


  if(!(command->background)) {
  int status;
  pid_t w = waitpid(pid, &status, 0);
  if (w == -1) {
    print_error("waitpid");
    return -1;
  }

  if (WIFEXITED(status)) return WEXITSTATUS(status);
  else if (WIFSIGNALED(status)) return WTERMSIG(status);
  else return -1;
  } 
  
  dshell->background_process++;
  return 0;
}

