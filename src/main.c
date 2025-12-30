
#define _POSIX_C_SOURCE 200809L

#include "dshell.h"
#include "jobs.h"
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
Shell* g_dshell = NULL;  // global pointer for signal handler

// SIGCHLD handler: mark job DONE only if all processes in its pgid are gone
void sigchld_handler(int sig, siginfo_t* info, void* ucontext) {
    (void)sig;
    (void)info;
    (void)ucontext;

    if (!g_dshell) return;

    pid_t pid;
    int status;

    // Reap all children that have changed state
    while ((pid = waitpid(-1, &status, WNOHANG | WUNTRACED | WCONTINUED)) > 0) {

        // Find the job corresponding to this pid's pgid
        pid_t pgid = getpgid(pid);
        for (int i = 0; i < g_dshell->curr_jobs; i++) {
            Job* job = g_dshell->jobs[i];
            if (!job) continue;

            if (job->pgid != pgid) continue;

            if (WIFSTOPPED(status)) {
                job->state = STOPPED;
            } else if (WIFCONTINUED(status)) {
                job->state = RUNNING;
            } else if (WIFEXITED(status) || WIFSIGNALED(status)) {
                // Check if any children remain in this pgid
                if (waitpid(-pgid, &status, WNOHANG) <= 0) {
                    job->state = DONE;
                }
            }
        }
    }

    if (pid == -1 && errno != ECHILD) {
        perror("waitpid");
    }
}

int main(void)
{
    Shell* dshell = shell_init();
    g_dshell = dshell;  // make shell visible to handler

    // Setup SIGCHLD handler
    struct sigaction sa;
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = sigchld_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    while (dshell->running) {
        shell_step(dshell);
    }

    shell_close(dshell);

    return 0;
}

