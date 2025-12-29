#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>    

#include "launcher.h"
#include "read_write.h"
#include "commands.h"

void child(Command* command) 
{

  if(!(command->args) || !(command->args[0])) {
    print_error("Invalid arguments");
    _exit(127);
  }
  
  if (command->background) {
    setpgid(0, 0);
  }

  if (command->infile) {
    int fd = open(command->infile, O_RDONLY);
    if (fd == -1) { print_error("error opening fd"); _exit(127); }
    if(dup2(fd, STDIN_FILENO) == -1) print_error("dup2 error");

    close(fd);
  }

  if (command->outfile) {
    int fd = open(command->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) { print_error("error opening fd"); _exit(127); }
    if(dup2(fd, STDOUT_FILENO) == -1) print_error("dup2 error");
    close(fd);
  }

  if(execvp(command->args[0], command->args) == -1) {
    print_error("Failed to start program");
    _exit(127);
  }


}

int launch_task(Command* command, Shell* dshell)
{
  pid_t pid;

  pid = fork();

  if(pid<0) {
    print_error("Fork failed");
    return -1;
  }

  if(pid==0) child(command); 

  if(command->background) {
    dshell->background_process++;
    return 0;
  }

  int status;
  pid_t w = waitpid(pid, &status, 0);

  if (w == -1) {
    print_error("waitpid");
    return -1;
  }

  if (WIFEXITED(status)) return WEXITSTATUS(status);
  else if (WIFSIGNALED(status)) return WTERMSIG(status);
  
  return -1;

}
