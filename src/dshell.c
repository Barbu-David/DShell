#define _POSIX_C_SOURCE 200809L
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>


#include "dshell.h"
#include "ui.h"
#include "read_write.h"
#include "line_tokenizer.h"
#include "builtins.h"
#include "args.h"
#include "sf_wraps.h"

Shell* shell_init() {

  print_banner(PURPLE);

  Shell* dshell = (Shell*) sf_malloc(sizeof(Shell));

  dshell->historyCommand = init_command();
  dshell->running = true;
  dshell->curr_jobs=0;
  dshell->job_capacity=50;

  dshell->jobs= (Job**) sf_malloc(sizeof(Job*) * dshell->job_capacity);

  dshell->builtins=init_builtins(&(dshell->num_builtins));

  return dshell;
}

void shell_close(Shell* dshell) 
{
  free_command(dshell->historyCommand);
 
  if (dshell->jobs) {
    for (int i = 0; i < dshell->curr_jobs; i++) {
      Job* job = dshell->jobs[i];
      if (job) {
        if (job->pgid > 0) {
          kill(-job->pgid, SIGKILL);
          waitpid(-job->pgid, NULL, 0);  // reap immediately
        }
        free_job(job);
      }
    }
    free(dshell->jobs);
  }
  free(dshell);
}

void shell_step(Shell* dshell) 
{
  print_shell_prompt();

  char* line = read_line();
  char** args = tokenize_line(line);
  Job* job = build_job(args, dshell);

  add_job(dshell, job);

  int status = launch_job(job, dshell);
  if(status == -1 && line != NULL && job && !job->background) {
    print_error("Failed to execute program");
    remove_job(dshell, job);
    free_job(job);
  }
  free(line);
}

void add_job(Shell* dshell, Job* job)
{

  if(dshell->curr_jobs >= dshell->job_capacity || !job) return; // TO DO add reallocation
  job->id=dshell->curr_jobs;
  for(int i=0; i<job->command_num; i++) job->commands[i]->job_id=job->id;

  dshell->jobs[dshell->curr_jobs++]=job;

}

void remove_job(Shell* dshell, Job* job)
{
  if (!dshell || !job) return;
  int idx = job->id;

  if (idx < 0 || idx >= dshell->curr_jobs) return;

  dshell->curr_jobs--; 
  dshell->jobs[idx] = dshell->jobs[dshell->curr_jobs]; 
  if (dshell->jobs[idx]) dshell->jobs[idx]->id = idx;    
  dshell->jobs[dshell->curr_jobs] = NULL;
}

void reap_background_jobs(struct Shell *dshell) {
  if (!dshell) return;

  for (int i = 0; i < dshell->curr_jobs;) {
    Job *job = dshell->jobs[i];
    if (!job) {
      i++;
      continue;
    }

    if (!job->background && job->state != DONE) {
      i++;
      continue;
    }

    int status;
    pid_t w;

    if (job->background && job->state == RUNNING) {
      while ((w = waitpid(-job->pgid, &status, WNOHANG)) > 0) {
        // Update job state if needed
        job->state = DONE;
      }

      if (w == 0) {
        i++;
        continue;
      } else if (w < 0 && errno != ECHILD) {
        print_error("waitpid(-pgid, WNOHANG) failed");
        print_error(strerror(errno));
        i++;
        continue;
      }
    }

    remove_job(dshell, job);
    free_job(job);
  }
}


