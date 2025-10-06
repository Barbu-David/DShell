#include "dshell.h"

int main(void)
{
  
  print_banner(PURPLE);
  int background_process = 0;
  while(1) {

    print_shell_prompt();
    char* line = read_line();
    char** args = tokenize_line(line);
  
    int status = dsh_execute(args);
    
    if(status == -1) print_error("Failed to execute program");
    if(status == BPROCESS) background_process++;
    
    if(background_process) background_process = reap_background_process(background_process); 

    free(args);
    free(line);

  }

  return 0;
}
