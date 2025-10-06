#include "launcher.h"
#include "read_write.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int launch_task(char** args) 
{
  pid_t pid;

  pid = fork();

  if(pid<0) {
    print_error("Fork failed");
    return -1;
  }

  if(pid==0) {

    if(!args || !args[0]) {
      print_error("Invalid arguments");
      _exit(127);
    }

    if(execvp(args[0], args) == -1) {
      print_error("Failed to start program");
      _exit(127);
    }
  }


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

int launch_task_nb(char** args) 
{
  pid_t pid;

  pid = fork();

  if(pid<0) {
    print_error("Fork failed");
    return -1;
  }

  if(pid==0) {

    if(!args || !args[0]) {
      print_error("Invalid arguments");
      _exit(127);
    }

    if(execvp(args[0], args) == -1) {
      print_error("Failed to start program");
      _exit(127);
    }
  }

  return BPROCESS;
}
