#include <sys/wait.h>
#include <errno.h>
#include <string.h>

#include "read_write.h"
#include "background.h"

void reap_background_process(Shell* dshell) {
    int status;
    pid_t pid;

    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        dshell->background_process--;
    }

    if (pid == -1 && errno != ECHILD) {
        print_error(strerror(errno));
        print_error("Background process waitpid failed");
  }
}
