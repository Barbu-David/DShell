#include "commands.h"
#include "args.h"
#include "sf_wraps.h"
#include "read_write.h"

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>    
#include <errno.h>
#include <string.h>
#include <stdlib.h>

void free_command(Command* c)
{
  free_args(c->args);
  if(c->infile) free(c->infile);
  if(c->outfile) free(c->outfile);
  free(c);
}

Command* init_command()
{
  
  Command* command = (Command*) sf_malloc(sizeof(Command));
  command->execute = NULL;
  command->infile=NULL;
  command->outfile=NULL;
  command->to_history = true;
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

  if (dst->infile) free(src->infile); 
  if (dst->outfile) free(src->outfile);
   
  dst->to_history  = src->to_history;
  dst->execute     = src->execute;

  if(src->infile) dst->infile = sf_strdup(src->infile);
  if(src->outfile) dst->outfile = sf_strdup(src->outfile);

  dst->args = copy_args(src->args);
}

void child(Command* command) 
{
  if (!(command->args) || !(command->args[0])) {
    print_error("Invalid arguments");
    _exit(127);
  }

  if (command->infile) {
    if (strncmp(command->infile, "FD:", 3) == 0) {
      int fd = atoi(command->infile + 3);
      if (dup2(fd, STDIN_FILENO) == -1) {
        print_error("dup2 error for infile");
        print_error(strerror(errno));
        _exit(127);
      }
      // don't close(fd) here — it's fine to close, but dup2 duplicates it
      //close(fd);
    } else {
      int fd = open(command->infile, O_RDONLY);
      if (fd == -1) {
        print_error("error opening fd");
        print_error(strerror(errno));
        _exit(127);
      }
      if (dup2(fd, STDIN_FILENO) == -1) {
        print_error("dup2 error");
        print_error(strerror(errno));
        _exit(127);
      }
      close(fd);
    }
  }

  if (command->outfile) {
    if (strncmp(command->outfile, "FD:", 3) == 0) {
      int fd = atoi(command->outfile + 3);
      if (dup2(fd, STDOUT_FILENO) == -1) {
        print_error("dup2 error for outfile");
        print_error(strerror(errno));
        _exit(127);
      }
      //close(fd);
    } else {
      int fd = open(command->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
      if (fd == -1) {
        print_error("error opening fd");
        print_error(strerror(errno));
        _exit(127);
      }
      if (dup2(fd, STDOUT_FILENO) == -1) {
        print_error("dup2 error");
        print_error(strerror(errno));
        _exit(127);
      }
      close(fd);
    }
  }

  if (execvp(command->args[0], command->args) == -1) {
    print_error(strerror(errno));
    print_error("Failed to start program");
    _exit(127);
  }
}
/*
void child(Command* command) 
{

  if(!(command->args) || !(command->args[0])) {
    print_error("Invalid arguments");
    _exit(127);
  }
 
  /*
  if (command->background) {
    setpgid(0, 0);
  }
  

  if (command->infile) {
    int fd = open(command->infile, O_RDONLY);
    if (fd == -1) {
      print_error("error opening fd");
      print_error(strerror(errno));
      _exit(127); 
    }
    if (dup2(fd, STDIN_FILENO) == -1) {
      print_error("dup2 error");
      print_error(strerror(errno));
      _exit(127);
    }

    close(fd);
  }

  if (command->outfile) {
    int fd = open(command->outfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
      print_error("error opening fd");
      print_error(strerror(errno));
      _exit(127); 
    }
    if (dup2(fd, STDOUT_FILENO) == -1) {
      print_error("dup2 error");
      print_error(strerror(errno));
      _exit(127);
    }
    close(fd);
  }

  if(execvp(command->args[0], command->args) == -1) {
    print_error(strerror(errno));
    print_error("Failed to start program");
    _exit(127);
  }

}
*/
int launch_command(Command* command, Shell* dshell)
{

  pid_t pid;

  pid = fork();

  if(pid<0) {
    print_error("Fork failed");
    print_error(strerror(errno));
    return -1;
  }

  if(pid==0) child(command); 
/*
  if(command->background) {
    dshell->background_process++;
    return 0;
  }
*/
  int status;
  pid_t w = waitpid(pid, &status, 0);

  if (w == -1) {
    print_error(strerror(errno));
    print_error("waitpid");
    return -1;
  }

  if (WIFEXITED(status)) return WEXITSTATUS(status);
  else if (WIFSIGNALED(status)) return WTERMSIG(status);
  
  return -1;

}

void assign_executor(const char* c, Shell* dshell, Command* command)
{
  for (int i = 0; i < dshell->num_builtins; i++) 
    if (strcmp(c, dshell->builtin_str[i]) == 0) {
      command->execute = dshell->builtin_func[i];
      return;
    }
  command->execute =launch_command;
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
