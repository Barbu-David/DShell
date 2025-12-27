#include "dshell.h"

int main(void)
{

  Shell* dshell = shell_init();

  while(dshell->running) {
    shell_step(dshell);
  }

  shell_close(dshell);

  return 0;
}
