#define _POSIX_C_SOURCE 200809L

#include "dshell.h"
#include "dsh_execute.h"
#include "launcher.h"
#include "read_write.h"
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ui.h"
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>

char *builtin_str[] = {
  "help",
  "cd",
  "banner",
  "!!",
  "exit"
};

int (*builtin_func[]) (char **, Shell*) = {
  &dsh_help,
  &dsh_cd,
  &dsh_banner,
  &dsh_history,
  &dsh_exit
};

int dsh_num_builtins() {
  return sizeof(builtin_str) / sizeof(char *);
}

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

int dsh_banner(char** args, Shell* dshell)
{
  (void) args;
  (void) dshell;
  print_banner(PURPLE);
  return 0;
}

int dsh_cd(char** args, Shell* dshell)
{  
  (void) dshell;
  if (args[1] == NULL) print_error("lsh: expected argument to \"cd\"\n");
  else if (chdir(args[1]) != 0) print_error("chdir failed");
  return 0;
}

int dsh_help(char** args, Shell* dshell) 
{
  (void) dshell;
  (void) args;
  printf("Hello, this is dshell. It has the following builtins:\n");
  for(int i=0; i< dsh_num_builtins(); i++) printf("%s\n", builtin_str[i]);
  printf("Type program names and arguments, and hit enter.\n");
  return 0;
}

int dsh_exit(char** args, Shell* dshell)
{
  (void) args;
  dshell->running=false;
  return 0;
}

int dsh_execute(char **args, Shell* dshell)
{

  if (args[0] == NULL) return 0;

  int status;
  for (int i = 0; i < dsh_num_builtins(); i++) 
    if (strcmp(args[0], builtin_str[i]) == 0) {
      status=(*builtin_func[i])(args, dshell);
      if(*builtin_func[i] == dsh_history) goto ret_no_update;
      goto ret;
    }

  char *infile = NULL;
  char *outfile = NULL;
  bool background = false;

  int j = 0;
  char *argv[BUFSIZ]; 
  for (int i = 0; args[i]; i++) {

    if (strcmp(args[i], "<") == 0) {
      if (!args[i+1]) { print_error("Syntax error: no input file"); return -1; }
      infile = args[++i];
    }
    else if (strcmp(args[i], ">") == 0) {
      if (!args[i+1]) { print_error("Syntax error: no output file"); return -1; }
      outfile = args[++i];
    }
    else if (strcmp(args[i], "&") == 0 && !args[i+1]) {
      background = true;
      dshell->background_process++;
    }
    else {
      argv[j++] = args[i];
    }
  }
  argv[j] = NULL;

  status= launch_task(argv, infile, outfile, !background);

ret:
  char **new_copy = copy_args(args);  
  free_args(dshell->history_args);
  dshell->history_args = new_copy;       

ret_no_update:

  return status;
}

void reap_background_process(Shell* dshell) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        printf("[Background process %d finished]\n", pid);
        dshell->background_process--;
    }

    if (pid == -1 && errno != ECHILD)
        print_error("Background process waitpid failed");
}



int dsh_history(char ** args, Shell* dshell) 
{
  (void)args;

  if(dshell->history_args==NULL) {
    print_error("Empty history");
    return 1;
  }

  return dsh_execute(dshell->history_args, dshell);
}


