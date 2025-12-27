#ifndef DSHELL_H
#define DSHELL_H

#include <stdbool.h>

typedef struct {
  bool running;
  int background_process;
  char** history_args;
} Shell;

Shell* shell_init();
void shell_step(Shell* dshell);
void shell_close(Shell* dshell);

#endif 

