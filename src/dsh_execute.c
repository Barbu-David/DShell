#include "dsh_execute.h"
#include "launcher.h"
#include "read_write.h"
#include "banner.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "colors.h"
#include <sys/wait.h>
#include <errno.h>

char *builtin_str[] = {
  "help",
  "cd",
  "banner",
  "exit"

};

int (*builtin_func[]) (char **) = {
  &dsh_help,
  &dsh_cd,
  &dsh_banner,
  &dsh_exit
};

int dsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

int dsh_banner(char** args)
{
  (void) args;
  print_banner(PURPLE);
  return 0;
}
int dsh_cd(char** args)
{
  if (args[1] == NULL) print_error("lsh: expected argument to \"cd\"\n");
  else if (chdir(args[1]) != 0) print_error("chdir failed");
  return 0;
}

int dsh_help(char** args) 
{
  (void) args;
  printf("Hello, this is dshell. It has the following builtins:\n");
  for(int i=0; i< dsh_num_builtins(); i++) printf("%s\n", builtin_str[i]);
  printf("Type program names and arguments, and hit enter.\n");
  return 0;
}

int dsh_exit(char** args)
{
  free(args);
  exit(0);
}

int dsh_execute(char **args)
{
  if (args[0] == NULL) return 0;

  for (int i = 0; i < dsh_num_builtins(); i++) 
    if (strcmp(args[0], builtin_str[i]) == 0) return (*builtin_func[i])(args);

  for(int idx = 0; args[idx]; idx++) if(strcmp(args[idx], "&") == 0 && !args[idx+1]) {
    args[idx]=NULL;
    return launch_task_nb(args);
  }

  return launch_task(args);
}

int reap_background_process(int background_process) 
{

  int status;
  int i;
  do {
  i = waitpid(-1, &status, WNOHANG);
  if(i==-1) {

      if(errno!=ECHILD) print_error("Background process failed");
      background_process--;
      i=0;
    }
  if(i>0) background_process--;
  } while(i); 

  return background_process;

}

