#define _POSIX_C_SOURCE 200809L
#include "args.h"
#include <stdlib.h>
#include <string.h>
#include "read_write.h"
#include <stdio.h>

char** copy_args(char **args) {
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

void free_args(char **args) 
{
  if (!args) return;
  free(args);
}

char** init_args(size_t count)
{
  char **args = malloc((count + 1) * sizeof(char *));
  if (!args) {
    print_error("malloc failed");
    exit(1);
  }

  for (size_t i = 0; i <= count; i++) {
    args[i] = NULL;
  }

  return args;
}

void free_args_deep(char **args)
{
  if (!args) return;
  for (size_t i = 0; args[i]; i++) {
    free(args[i]);
  }
  free(args);
}
