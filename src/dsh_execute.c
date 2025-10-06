#define _POSIX_C_SOURCE 200809L
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
  "!!",
  "exit"

};

int (*builtin_func[]) (char **) = {
  &dsh_help,
  &dsh_cd,
  &dsh_banner,
  &dsh_history,
  &dsh_exit
};

int dsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

char** last_args = NULL;

char ** copy_args(char **args) {
  if (!args) return NULL;

  int count = 0;
  while (args[count]) count++;  

  char **copy = malloc((count + 1) * sizeof(char*));
  if (!copy) {
    print_error("malloc failed");
    exit(1);
  }

  for (int i = 0; i < count; i++) {
    copy[i] = strdup(args[i]);
    if (!copy[i]) {
      print_error("strdup");
      exit(1);
    }
  }
  copy[count] = NULL;
  return copy;
}

void free_args(char **args) {
  if (!args) return;
  for (int i = 0; args[i]; i++) free(args[i]);
  free(args);
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
  free_args(last_args);
  free(args);
  exit(0);
}

int dsh_execute(char **args)
{

  if (args[0] == NULL) return 0;

  int status;
  for (int i = 0; i < dsh_num_builtins(); i++) 
    if (strcmp(args[0], builtin_str[i]) == 0) {
      status=(*builtin_func[i])(args);
      if(*builtin_func[i] == dsh_history) goto ret_no_update;
      goto ret;
    }

  for(int idx = 0; args[idx]; idx++) if(strcmp(args[idx], "&") == 0 && !args[idx+1]) {
    args[idx]=NULL;
    status= launch_task_nb(args);
    goto ret;
  }

  status= launch_task(args);

  ret:
  char **new_copy = copy_args(args);  
  free_args(last_args);
  last_args = new_copy;       

  ret_no_update:

  return status;
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

int dsh_history(char ** args) 
{
  (void)args;

  if(last_args==NULL) {
    print_error("Empty history");
    return 1;
  }

  return dsh_execute(last_args);
}


