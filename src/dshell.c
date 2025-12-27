#include <stdlib.h>
#include <string.h>

#include "dshell.h"
#include "ui.h"
#include "read_write.h"
#include "line_tokenizer.h"
#include "dsh_execute.h"

Shell* shell_init() 
{

  print_banner(PURPLE);
  Shell* dshell = (Shell*) malloc(sizeof(Shell));

  if(!dshell) print_error("malloc failed");

  dshell->running=true;
  dshell->background_process=0;
  dshell->history_args=NULL;

  return dshell;
}

void shell_close(Shell* dshell) 
{
  free_args(dshell->history_args);
  free(dshell);
}

void shell_step(Shell* dshell) 
{
  print_shell_prompt();

  char* line = read_line();
  char** args = tokenize_line(line);
  int status = dsh_execute(args, dshell);

  if(status == -1) print_error("Failed to execute program");

  if(dshell->background_process) reap_background_process(dshell); 

  free(args);
  free(line);

}
