#define _GNU_SOURCE

#include "colors.h"
#include "read_write.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

void print_error(const char* error)
{
  fprintf(stderr, "%s%s%s\n", RED, error, RESET_COLOR);
  fflush(stdout);
}

void print_shell_prompt(void) 
{

  char cwd[BUFSIZ];
  if(getcwd(cwd, sizeof(cwd)) == NULL) print_error("getcwd failed");

  printf("%s%s dsh $ %s", BLUE, cwd, RESET_COLOR);
  fflush(stdout);
}

char* read_line(void) 
{
  char *buf = NULL; 
  size_t buf_size;

  if (getline(&buf, &buf_size, stdin) == -1) {

    if(feof(stdin)) exit(0);
    
    print_error("getline failed");  
    free(buf);

    exit(1);

  }

  return buf;
}
