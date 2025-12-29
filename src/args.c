#include <stdlib.h>

#include "args.h"
#include "sf_wraps.h"

char** copy_args(char **args) {

  if (!args) return NULL;

  int count = 0;
  while (args[count]) count++;  

  char** copy = (char**) sf_malloc((count + 1) * sizeof(char*));

  for (int i = 0; i < count; i++) copy[i] = sf_strdup(args[i]);
  
  copy[count] = NULL;

  return copy;
}

char** init_args(size_t count)
{
  char** args = (char**) sf_malloc((count + 1) * sizeof(char *));

  for (size_t i = 0; i <= count; i++) args[i] = NULL;

  return args;
}

void free_args(char **args)
{
  if (!args) return;
  for (size_t i = 0; args[i]; i++) free(args[i]);
  
  free(args);
}
