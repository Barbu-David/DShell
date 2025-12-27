#include "launcher.h"
#include "read_write.h"
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

int launch_task(char** args, const char *infile, const char *outfile, bool blocking)
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
         if (!blocking) {
         setpgid(0, 0); // new process group for background
         }
        if (infile) {
            int fd = open(infile, O_RDONLY);
            if (fd == -1) { print_error("open"); _exit(127); }
            dup2(fd, STDIN_FILENO); //TO DO add error checking
            close(fd);
        }

        if (outfile) {
            int fd = open(outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            if (fd == -1) { print_error("open"); _exit(127); }
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

    if(execvp(args[0], args) == -1) {
      print_error("Failed to start program");
      _exit(127);
    }
  }


  if(blocking) {
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

  return BPROCESS;
}

